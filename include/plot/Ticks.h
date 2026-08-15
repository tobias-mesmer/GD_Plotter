#pragma once
#include <array>
#include <string>
#include <vector>
#include "glm/vec3.hpp"

namespace gdp::plot {
    struct Label {
        std::string text;
        glm::vec3 anchor;
    };

    struct AxisTicks {
        std::vector<Label> labels;
        glm::vec3 edgeStart, edgeEnd; // the edge of the axis the ticks sit on, for screen-space fir
        glm::vec3 outward;
    };

    struct TickResult {
        std::vector<glm::vec3> segments;
        std::array<size_t, 3>  segmentCountPerAxis;
        std::array<AxisTicks, 3> axes;
    };

    TickResult ticks(glm::vec3 min, glm::vec3 max, unsigned int ticksX, unsigned int ticksY, unsigned int ticksZ);

    // http://vis.stanford.edu/files/2010-TickLabels-InfoVis.pdf
    std::vector<glm::vec3> extendedWilkinson();
}
