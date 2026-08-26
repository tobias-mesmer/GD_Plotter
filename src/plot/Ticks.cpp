#include "plot/Ticks.h"

#include <format>
#include <vector>

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

    plot::TickResult plot::ticks(const glm::vec3& min, const glm::vec3& max, unsigned int ticksX, unsigned int ticksY, unsigned int ticksZ, Selection selection, const TickMode mode) {
        switch (mode) {
            case EXTENDED_WILKINSON:
                return extendedWilkinson(min, max, ticksX, ticksY, ticksZ, selection);
            case NORMAL:
            default:
                return ticksNormal(min, max, ticksX, ticksY, ticksZ, selection);
        }
    }

    // http://vis.stanford.edu/files/2010-TickLabels-InfoVis.pdf
    plot::TickResult plot::extendedWilkinson(const glm::vec3& min, const glm::vec3& max, const unsigned int ticksX, const unsigned int ticksY, const unsigned int ticksZ,
                                             Selection selection) {
        return TickResult{};
    }

    // Generate tick segments pointing away from the box and labelling from AxisBox extents, spaced by steps derived from the number of prescribed ticks per axis.
    plot::TickResult plot::ticksNormal(const glm::vec3& min, const glm::vec3& max, const unsigned int ticksX, const unsigned int ticksY, const unsigned int ticksZ, Selection selection) {
        TickResult result{};
        result.segmentCountPerAxis = {ticksX, ticksY, ticksZ};

        const unsigned counts[3] = {ticksX, ticksY, ticksZ};
        for (int a = 0; a < 3; ++a) {
            auto& [labels, edgeStart, edgeEnd, outward] = result.axes[a];
            glm::vec3 minA = min;

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

            const float step = (maxA[a] - minA[a]) / static_cast<float>(counts[a] - 1);

            for (unsigned i = 0; i < counts[a]; ++i) {
                glm::vec3 inner = minA;
                inner[a] += step * i;
                const glm::vec3 outer = inner + outward;
                result.segments.push_back(inner);
                result.segments.push_back(outer);
                labels.emplace_back(std::format("{:.2g}", minA[a] + step * i), outer);
            }
        }

        return result;
    }
}
