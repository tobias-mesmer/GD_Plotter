#include "plot/Ticks.h"

#include <format>
#include <vector>

namespace gdp {

    plot::EdgeSelection plot::selectEdges(const glm::vec3& cameraViewDir, const glm::vec3& min, const glm::vec3& max) {
        EdgeSelection result{};

        result.backBit[0] = std::signbit(cameraViewDir.x) ? 0 : 1;
        result.backBit[1] = std::signbit(cameraViewDir.y) ? 0 : 1;
        result.backBit[2] = std::signbit(cameraViewDir.z) ? 0 : 1;

        return result;
    }

    plot::TickResult plot::ticks(const glm::vec3& min, const glm::vec3& max, unsigned int ticksX, unsigned int ticksY, unsigned int ticksZ, const TickMode mode) {
        switch (mode) {
            case EXTENDED_WILKINSON:
                return extendedWilkinson(min, max, ticksX, ticksY, ticksZ);
            case NORMAL:
            default:
                return ticksNormal(min, max, ticksX, ticksY, ticksZ);
        }
    }

    // http://vis.stanford.edu/files/2010-TickLabels-InfoVis.pdf
    plot::TickResult plot::extendedWilkinson(const glm::vec3& min, const glm::vec3& max, const unsigned int ticksX, const unsigned int ticksY, const unsigned int ticksZ) {
        return TickResult{};
    }

    // Generate tick segments pointing away from the box and labelling from AxisBox extents, spaced by steps derived from the number of prescribed ticks per axis.
    plot::TickResult plot::ticksNormal(const glm::vec3& min, const glm::vec3& max, const unsigned int ticksX, const unsigned int ticksY, const unsigned int ticksZ) {
        TickResult result{};
        result.segmentCountPerAxis = {ticksX, ticksY, ticksZ};

        const unsigned counts[3] = {ticksX, ticksY, ticksZ};
        for (int a = 0; a < 3; ++a) {
            AxisTicks& axis = result.axes[a];
            std::vector<glm::vec3>& segments = result.segments;
            axis.edgeStart = min;
            axis.edgeEnd = min;
            axis.edgeEnd[a] = max[a];

            const float step = (max[a] - min[a]) / static_cast<float>(counts[a] - 1);
            glm::vec3 outward(-0.2f);
            outward[a] = 0.0f; // stick out on the other two axes
            axis.outward = outward;

            for (unsigned i = 0; i < counts[a]; ++i) {
                glm::vec3 inner = min;
                inner[a] += step * i;
                const glm::vec3 outer = inner + outward;
                segments.push_back(inner);
                segments.push_back(outer);
                axis.labels.emplace_back(std::format("{:.2g}", min[a] + step * i), outer);
            }
        }

        return result;
    }
}
