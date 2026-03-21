#pragma once

#include <glm/glm.hpp>

namespace gdp::graphics {
    class Camera {
    public:
        void setPosition(const glm::dvec3& newPos) { m_position = newPos; }

        void setTarget(const glm::dvec3& newTarget) { m_target = newTarget; }

        void setAspectRatio(double aspect) { m_aspectRatio = aspect; }

        void setProjection(double fov, double nearPlane, double farPlane) {
            m_fov = fov;
            m_nearPlane = nearPlane;
            m_farPlane = farPlane;
        }

        void updateProjectionMatrix();

        void updateViewMatrix();

    private:
        glm::dvec3 m_position;
        glm::dvec3 m_target;
        glm::dvec3 m_up{0, 1, 0};

        double m_aspectRatio = 16.0 / 9.0;
        double m_fov = 45.0;
        double m_nearPlane = 0.1;
        double m_farPlane = 1000.0;

        glm::dmat4 m_projection;
        glm::dmat4 m_view;
    };
}
