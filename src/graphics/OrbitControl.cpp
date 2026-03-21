#include "../../include/graphics/OrbitControl.h"

#include <iomanip>
#include <GLFW/glfw3.h>

namespace gdp::graphics {
    void OrbitControl::update(GLFWwindow* window) {
        double x, y;
        glfwGetCursorPos(window, &x, &y);

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            const double dx = x - m_lastX;
            const double dy = y - m_lastY;

            m_yaw += dx * m_rotationSpeed;
            m_pitch += dy * m_rotationSpeed;

            if (m_pitch > 89.0f)
                m_pitch = 89.0f;
            if (m_pitch < -89.0f)
                m_pitch = -89.0f;

            updateCameraPosition();
        }

        m_lastX = x;
        m_lastY = y;
    }

    void OrbitControl::updateCameraPosition() const {
        glm::vec3 newPos = m_target +
                            glm::vec3(
                                m_radius * cos(m_pitch) * sin(m_yaw),
                                m_radius * sin(m_pitch),
                                m_radius * cos(m_pitch) * cos(m_yaw)
                            );

        m_camera.setPosition(newPos);
        m_camera.updateViewMatrix();
    }

    void OrbitControl::onScroll(double delta) {
        m_radius -= delta * m_zoomSpeed;
        updateCameraPosition();
    }
}
