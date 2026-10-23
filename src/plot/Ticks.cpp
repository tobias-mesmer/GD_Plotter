#include "plot/Ticks.h"

#include <algorithm>
#include <format>
#include <limits>
#include <optional>
#include <vector>
#include <cmath>

namespace gdp {
    // Derives camera octant for back pane indices, then indices for the tick label edges:
    // backBit[a] for an axis a encodes whether the  (all-positive octant = (0, 0, 0)).
    // Naturally matches the indices of the cube.
    plot::Selection plot::selectEdges(const glm::vec3& cameraPos, const glm::vec3& center) {
        Selection result{};
        for (int a = 0; a < 3; ++a) {
            result.backBit[a] = cameraPos[a] >= center[a] ? 0 : 1;
        }

        // Each label axis can sit at four positions encoded by whether the other two are set or not
        result.edgeCorner[0] = (result.backBit[1] << 1) | (!result.backBit[2] << 2); // x-axis
        result.edgeCorner[2] = !result.backBit[0] | (result.backBit[1] << 1); // z-axis

        // Deduce left-most option from the current camera octant (camera is always +y up and facing the world origin). Rotate octant by 90° in the xz plane.
        result.edgeCorner[1] = result.backBit[2] | (!result.backBit[0] << 2);

        return result;
    }

    namespace {
        // Weighted sum of the four criteria; defaults of 1 give the upper bounds used for pruning the search.
        // The parameter order follows the order in which they are added in extendedWilkinson(...).
        double score(const std::vector<double>& w, const double sm, const double dm = 1.0, const double cm = 1.0, const double l = 1.0) {
            return w[0] * sm + w[1] * cm + w[2] * dm + w[3] * l;
        }

        std::optional<std::vector<double> > seq(double start, double step, size_t count) {
            if (count == 0) return std::nullopt;

            std::vector<double> result;
            result.reserve(count);
            for (size_t i = 0; i < count; ++i) {
                result.push_back(start + static_cast<double>(i) * step);
            }
            return result;
        }

        // Rewards nicer numbers (earlier in Q), small skip factors j and including 0 as a label (v).
        double simplicity(const size_t qIndex, const size_t qCount, const double j, const double lmin, const double lmax, const double lstep) {
            const double eps = std::numeric_limits<double>::epsilon() * 100;

            // Floored modulo like R's %%, so negative lmin are handled correctly
            const double mod = lmin - lstep * std::floor(lmin / lstep);
            const double v = (mod < eps || lstep - mod < eps) && lmin <= 0 && lmax >= 0 ? 1.0 : 0.0;

            return 1.0 - static_cast<double>(qIndex) / static_cast<double>(qCount - 1) - j + v;
        }

        // Upper bound of simplicity for given q and j, assuming 0 is included (v = 1).
        double simplicity_max(const size_t qIndex, const size_t qCount, const double j) {
            return 1.0 - static_cast<double>(qIndex) / static_cast<double>(qCount - 1) - j + 1.0;
        }

        // Penalizes label ranges that extend beyond or fall short of the data range.
        double coverage(const double dmin, const double dmax, const double lmin, const double lmax) {
            const double range = dmax - dmin;
            return 1.0 - 0.5 * ((dmax - lmax) * (dmax - lmax) + (dmin - lmin) * (dmin - lmin)) / ((0.1 * range) * (0.1 * range));
        }

        // Upper bound of coverage for a label span, assuming it is centered on the data range.
        double coverage_max(const double dmin, const double dmax, const double span) {
            const double range = dmax - dmin;
            if (span > range) {
                const double half = (span - range) / 2.0;
                return 1.0 - 0.5 * (half * half + half * half) / ((0.1 * range) * (0.1 * range));
            }
            return 1.0;
        }

        // Compares the label density r against the target density rt for m labels.
        double density(const double k, const double m, const double dmin, const double dmax, const double lmin, const double lmax) {
            const double r = (k - 1.0) / (lmax - lmin);
            const double rt = (m - 1.0) / (std::max(lmax, dmax) - std::min(dmin, lmin));
            return 2.0 - std::max(r / rt, rt / r);
        }

        // Upper bound of density for k labels, given a target of m labels.
        double density_max(const double k, const double m) {
            if (k >= m) return 2.0 - (k - 1.0) / (m - 1.0);
            return 1.0;
        }

        // Font size, orientation and format are not optimized, so legibility is constant like in the R version.
        double legibility(double /*lmin*/, double /*lmax*/, double /*lstep*/) {
            return 1.0;
        }

        /* C++ port of the R reference implementation of the extended Wilkinson algorithm for positioning tick labels on axes.
         * Searches over nice step sizes and label ranges and returns the labeling with the highest weighted score of simplicity, coverage, density and legibility.
         *
         * Paper:  Talbot, J., Lin, S., Hanrahan, P. (2010) An Extension of Wilkinson's Algorithm for Positioning Tick Labels on Axes, InfoVis 2010.
         *         http://vis.stanford.edu/files/2010-TickLabels-InfoVis.pdf
         * R implementation by Justin Talbot: https://github.com/cran/labeling/blob/master/R/labeling.R
         *
         * @param dmin  minimum of the data range
         * @param dmax  maximum of the data range
         * @param m     target number of labels
         * @param loose if true, the labels must cover the data range (lmin <= dmin and lmax >= dmax)
         * @param Q     nice numbers, ordered by preference
         * @param w     weights of simplicity, coverage, density and legibility
         * @return label positions, or std::nullopt if no labeling was found
         */
        std::optional<std::vector<double> > extendedWilkinson(double dmin, double dmax, double m, const bool loose = false,
                                                              const std::vector<double>& Q = {1, 5, 2, 2.5, 4, 3},
                                                              const std::vector<double>& w = {0.25, 0.2, 0.5, 0.05}) {
            if (m < 2) return std::nullopt;

            auto epsDouble = std::numeric_limits<double>::epsilon();
            auto maxDouble = std::numeric_limits<double>::max();

            if (dmin > dmax) {
                const double temp = dmin;
                dmin = dmax;
                dmax = temp;
            }

            // A degenerate range only gets a single tick, instead of m ticks stacked on the same position like in R.
            if (dmax - dmin < epsDouble) return std::vector{dmin};

            // If the range is too large, let seq generate some equally spaced steps.
            if (dmax - dmin > std::sqrt(maxDouble)) return seq(dmin, (dmax - dmin) / (m - 1), static_cast<size_t>(m));

            struct Candidate {
                double lmin, lmax, lstep;
            };
            double bestScore = -2.0;
            std::optional<Candidate> best;

            double j = 1.0;
            while (j < maxDouble) {
                for (size_t qi = 0; qi < Q.size(); ++qi) {
                    double sm = simplicity_max(qi, Q.size(), j);

                    if (score(w, sm) < bestScore) {
                        j = maxDouble;
                        break;
                    }

                    unsigned int k = 2;
                    // Loop over tick counts
                    while (k < std::numeric_limits<unsigned int>::max()) {
                        double dm = density_max(k, m);

                        if (score(w, sm, dm) < bestScore) break;

                        double delta = (dmax - dmin) / (k + 1) / j / Q[qi];
                        double z = std::ceil(std::log10(delta));

                        while (z < maxDouble) {
                            double step = j * Q[qi] * std::pow(10, z);

                            double cm = coverage_max(dmin, dmax, step * (k - 1));

                            if (score(w, sm, dm, cm) < bestScore) break;

                            double minStart = std::floor(dmax / step) * j - (k - 1) * j;
                            double maxStart = std::ceil(dmin / step) * j;

                            if (minStart > maxStart) {
                                ++z;
                                continue;
                            }

                            for (double start = minStart; start <= maxStart; start += 1.0) {
                                double lmin = start * (step / j);
                                double lmax = lmin + step * (k - 1);
                                double lstep = step;

                                double s = simplicity(qi, Q.size(), j, lmin, lmax, lstep);
                                double c = coverage(dmin, dmax, lmin, lmax);
                                double g = density(k, m, dmin, dmax, lmin, lmax);
                                double l = legibility(lmin, lmax, lstep);

                                double sc = score(w, s, g, c, l);

                                if (sc > bestScore && (!loose || (lmin <= dmin && lmax >= dmax))) {
                                    best = Candidate{.lmin = lmin, .lmax = lmax, .lstep = lstep};
                                    bestScore = sc;
                                }
                            }
                            ++z;
                        }
                        ++k;
                    }
                }
                ++j;
            }

            return best.has_value()
                       ? seq(best->lmin, best->lstep, static_cast<size_t>(std::llround((best->lmax - best->lmin) / best->lstep)) + 1)
                       : std::nullopt;
        }

        // Number of decimals needed to show x exactly, e.g. 0.25 -> 2, 50 -> 0. Values that are not nice fall back to 2 decimals.
        int decimalsFor(const double x) {
            for (int d = 0; d <= 6; ++d) {
                const double scaled = std::abs(x) * std::pow(10.0, d);
                if (std::abs(scaled - std::round(scaled)) <= 1e-6 * scaled) return d;
            }
            return 2;
        }

        // Label decimals follow the tick step, or the value itself if there is only a single tick.
        int labelDecimals(const std::vector<double>& values) {
            if (values.empty()) return 0;
            return decimalsFor(values.size() >= 2 ? values[1] - values[0] : values[0]);
        }

        // Generate tick segments for axis a pointing away from the box, and labels at the given tick values.
        void placeAxisTicks(plot::TickResult& result, const int a, const std::vector<double>& values, const glm::vec3& min, const glm::vec3& max, const plot::Selection& selection) {
            auto& [labels, edgeStart, edgeEnd, outward] = result.axes[a];
            glm::vec3 minA = min;

            // See whether edgeCorner bits of other axes are set, and if so, take the max value instead of min for that axis
            for (int b = 0; b < 3; ++b) {
                if (b == a) continue;
                const bool atMax = selection.edgeCorner[a] >> b & 1;
                minA[b] = atMax ? max[b] : min[b];
                outward[b] = atMax ? 0.2f : -0.2f;
            }
            glm::vec3 maxA = minA;
            maxA[a] = max[a];
            edgeStart = minA;
            edgeEnd = maxA;

            const int decimals = labelDecimals(values);
            const double snap = values.size() >= 2 ? std::abs(values[1] - values[0]) * 1e-9 : 0.0;

            for (const double v: values) {
                glm::vec3 inner = minA;
                inner[a] = static_cast<float>(v);
                const glm::vec3 outer = inner + outward;
                result.segments.push_back(inner);
                result.segments.push_back(outer);

                // Snap rounding residue like 1e-17 to 0, so the label doesn't show -0 or 1e-17
                const double labelValue = std::abs(v) < snap ? 0.0 : v;
                labels.emplace_back(std::format("{:.{}f}", labelValue, decimals), outer);
            }
            result.tickCountPerAxis[a] = values.size();
        }
    }

    plot::TickResult plot::ticks(const glm::vec3& min, const glm::vec3& max, unsigned int ticksX, unsigned int ticksY, unsigned int ticksZ, const Selection selection, const TickMode mode) {
        TickResult result{};
        const std::array counts{ticksX, ticksY, ticksZ};

        for (int a = 0; a < 3; ++a) {
            const double dmin = min[a];
            const double dmax = max[a];

            std::optional<std::vector<double> > values;
            if (mode == EXTENDED_WILKINSON) {
                values = extendedWilkinson(dmin, dmax, counts[a]);

                // Drop ticks outside the extents, as there is no axis edge to place them on
                if (values) {
                    const size_t before = values->size();
                    const double tolerance = (dmax - dmin) * 1e-9;
                    std::erase_if(*values, [&](const double v) { return v < dmin - tolerance || v > dmax + tolerance; });

                    // Not enough ticks left to show a scale
                    if (values->size() < 2 && values->size() < before) values.reset();
                }
            }

            // Fall back to equally spaced ticks if Wilkinson finds no labeling
            if (!values) values = seq(dmin, counts[a] > 1 ? (dmax - dmin) / (counts[a] - 1) : 0.0, counts[a]);
            if (!values) continue;

            placeAxisTicks(result, a, *values, min, max, selection);
        }
        return result;
    }
}
