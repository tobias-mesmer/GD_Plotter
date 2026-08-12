#include "plot/Ticks.h"
#include <vector>

namespace gdp {
    // http://vis.stanford.edu/files/2010-TickLabels-InfoVis.pdf
    std::vector<glm::vec3> plot::extendedWilkinson() {
        return std::vector<glm::vec3>{};
    }

    std::vector<glm::vec3> plot::ticks(glm::vec3 min, glm::vec3 max, unsigned int ticksX, unsigned int ticksY, unsigned int ticksZ) {
        std::vector<glm::vec3> ticks{};
        float stepX = (max.x - min.x) / static_cast<float>(ticksX - 1);
        float stepY = (max.y - min.y) / static_cast<float>(ticksY - 1);
        float stepZ = (max.z - min.z) / static_cast<float>(ticksZ - 1);

        for (unsigned int i = 0; i < ticksX; i++) {
            ticks.emplace_back(min + glm::vec3(i * stepX, 0.f, -0.1f));
            ticks.emplace_back(min + glm::vec3(i * stepX, 0.f, 0.1f));
        }

        for (unsigned int i = 0; i < ticksY; i++) {
            ticks.emplace_back(min + glm::vec3(-0.1f, i * stepY, 0.f));
            ticks.emplace_back(min + glm::vec3(0.1f, i * stepY, 0.f));
        }

        // TODO Depends on view direction
        for (unsigned int i = 0; i < ticksZ; i++) {
            ticks.emplace_back(min + glm::vec3(-0.1f, 0.f, i * stepZ));
            ticks.emplace_back(min + glm::vec3(0.1f, 0.f, i * stepZ));
        }

        return ticks;
    }
}
