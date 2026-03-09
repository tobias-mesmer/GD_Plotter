#pragma once

#include <glm/glm.hpp>
#include "glm/ext/matrix_transform.hpp"

namespace gdp::graphics {
    class Camera {
    private:
        glm::dvec3 position;
        glm::dmat4 rotation;
        glm::dvec3 up;

        glm::dvec3 target;
        float radius = 10.f;

        glm::dmat4 world;
        glm::dmat4 view;
        glm::dmat4 projection;
    };
}
