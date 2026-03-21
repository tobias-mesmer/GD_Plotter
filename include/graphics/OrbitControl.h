#pragma once

#include "Camera.h"
#include <GLFW/glfw3.h>

namespace gdp::graphics {
    class OrbitControl {
    public:
        explicit OrbitControl(Camera& camera) : m_camera(camera) {}

        void update(GLFWwindow* window);

        void onScroll(double yOffset);

    private:
        Camera& m_camera;

        glm::vec3 m_target{0.0, 0.0, 0.0};

        double m_lastX{0};
        double m_lastY{0};
        double m_pitch{0};
        double m_yaw{0};
        double m_radius{10.0};
        double m_rotationSpeed{0.005};
        double m_zoomSpeed{1.0};

        void updateCameraPosition() const;
    };
}
