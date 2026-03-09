#pragma once

#include <glm/glm.hpp>

namespace gdp::graphics {
    struct Theme {
        struct Grid {
            glm::vec4 color = {0.3f, 0.3f, 0.3f, 1.0f};
            float spacing = 1.0f;
            float thickness = 1.0f;
        } grid;

        struct Background {
            glm::vec4 color = {0.18f, 0.18f, 0.20f, 1.0f};
        } background;

        struct Axis {
            glm::vec4 xColor = {1, 0, 0, 1};
            glm::vec4 yColor = {0, 1, 0, 1};
            glm::vec4 zColor = {0, 0, 1, 1};
            float thickness = 2.0f;
        } axis;
    };
}