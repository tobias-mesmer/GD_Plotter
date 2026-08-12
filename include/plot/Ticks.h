#pragma once
#include <vector>
#include "glm/vec3.hpp"

namespace gdp::plot {

    std::vector<glm::vec3> ticks(glm::vec3 min, glm::vec3 max, unsigned int m_ticks_x, unsigned int m_ticks_y, unsigned int m_ticks_z);

    // http://vis.stanford.edu/files/2010-TickLabels-InfoVis.pdf
    std::vector<glm::vec3> extendedWilkinson();
}
