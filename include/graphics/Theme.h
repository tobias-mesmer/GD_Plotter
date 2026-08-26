#pragma once

#include <glm/glm.hpp>

namespace gdp::graphics::theme {
    struct Grid {
        glm::vec4 color = {0.3f, 0.3f, 0.3f, 1.0f};
        float spacing = 1.0f;
        float thickness = 1.0f;
    };

    struct Background {
        glm::vec4 color = {0.18f, 0.18f, 0.20f, 1.0f};
    };

    struct Axis {
        glm::vec4 xColor = {1, 0, 0, 1};
        glm::vec4 yColor = {0, 1, 0, 1};
        glm::vec4 zColor = {0, 0, 1, 1};
        float thickness = 2.0f;
    };

    struct AxisBox {
        glm::vec4 boxColor = glm::vec4(glm::vec3(0.7), 1);
        glm::vec4 paneColor = glm::vec4(glm::vec3(0.3), 1);
        glm::vec4 ticksColorX = glm::vec4(glm::vec3(0.84, 0.24, 0.30), 1.0);
        glm::vec4 ticksColorY = glm::vec4(glm::vec3(0.55, 0.75, 0.25), 1.0);
        glm::vec4 ticksColorZ = glm::vec4(glm::vec3(0.25, 0.45, 0.85), 1.0);
        glm::vec4 tickLabelsColor = glm::vec4(glm::vec3(0.7), 1);
    };

    inline Grid grid;
    inline Background background;
    inline Axis axis;
    inline AxisBox axis_box;
}