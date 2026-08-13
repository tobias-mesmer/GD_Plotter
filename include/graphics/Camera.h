#pragma once

#include <glm/glm.hpp>

namespace gdp::graphics {
    class Camera {
    public:
        void setPosition(const glm::vec3& newPos) { m_position = newPos; }

        void setTarget(const glm::vec3& newTarget) { m_target = newTarget; }

        void setAspectRatio(double aspect) { m_aspectRatio = aspect; }

        void setProjection(double fov, double nearPlane, double farPlane) {
            m_fov = fov;
            m_nearPlane = nearPlane;
            m_farPlane = farPlane;
        }

        void updateProjectionMatrix();

        void updateViewMatrix();

        [[nodiscard]] const glm::mat4& viewMatrix() const {return m_view;}
        [[nodiscard]] const glm::mat4& projectionMatrix() const {return m_projection;}

    private:
        glm::vec3 m_position;
        glm::vec3 m_target;
        glm::vec3 m_up{0, 1, 0};

        double m_aspectRatio = 16.0 / 9.0;
        double m_fov = 45.0;
        double m_nearPlane = 0.01;
        double m_farPlane = 1000.0;

        glm::mat4 m_projection;
        glm::mat4 m_view;
    };
}
