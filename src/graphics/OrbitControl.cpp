#include "graphics/OrbitControl.h"

#include <algorithm>
#include <iomanip>

namespace gdp::graphics {
    void OrbitControl::update(GLFWwindow* window) {
        double x, y;
        glfwGetCursorPos(window, &x, &y);

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
            const double dx = x - m_lastX;
            const double dy = y - m_lastY;

            m_yaw += fmod(dx * m_rotationSpeed, 360.0);
            m_pitch += dy * m_rotationSpeed;

            m_pitch = std::min<double>(m_pitch, 89.0f);
            m_pitch = std::max<double>(m_pitch, -89.0f);

            updateCameraPosition();
        }

        m_lastX = x;
        m_lastY = y;
    }

    void OrbitControl::updateCameraPosition() const {
        const double yawRad = glm::radians(-m_yaw);
        const double pitchRad = glm::radians(m_pitch);

        const glm::vec3 newPos = m_target +
                           glm::vec3(
                               m_radius * cos(pitchRad) * sin(yawRad),
                               m_radius * sin(pitchRad),
                               m_radius * cos(pitchRad) * cos(yawRad)
                           );

        m_camera.setPosition(newPos);
        m_camera.updateViewMatrix();
    }

    void OrbitControl::onScroll(double delta) {
        m_radius -= delta * m_zoomSpeed;
        m_radius = glm::clamp(m_radius, m_minRadius, m_maxRadius);
        updateCameraPosition();
    }
}
