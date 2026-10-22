#pragma once

#include "glm/vec3.hpp"
#include <vector>

#include "plot/Ticks.h"

namespace gdp::plot {

    std::vector<glm::vec3> grid(const glm::vec3& min, const glm::vec3& max, const TickResult& ticks, Selection selection);

}
