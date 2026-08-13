#pragma once
#include <array>
#include <string>
#include <vector>
#include "glm/vec3.hpp"

namespace gdp::plot {
    struct Label {
        std::string text;
        glm::vec3 inner; // starting point closer to AxisBox
        glm::vec3 outer; // text-facing point
    };

    struct TickResult {
        std::vector<glm::vec3> segments;
        std::vector<Label> labels;
        std::array<size_t, 3> segmentCountPerAxis;
    };

    TickResult ticks(glm::vec3 min, glm::vec3 max, unsigned int ticksX, unsigned int ticksY, unsigned int ticksZ);

    // http://vis.stanford.edu/files/2010-TickLabels-InfoVis.pdf
    std::vector<glm::vec3> extendedWilkinson();
}
