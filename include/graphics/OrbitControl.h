#pragma once

#include "Camera.h"
#include <GLFW/glfw3.h>

namespace gdp::graphics {
    class OrbitControl {
    public:
        explicit OrbitControl(Camera& camera) : m_camera(camera) {updateCameraPosition();}

        void update(GLFWwindow* window);

        void onScroll(double yOffset);

    private:
        Camera& m_camera;

        glm::vec3 m_target{0.0, 0.0, 0.0};

        double m_lastX{0};
        double m_lastY{0};
        double m_pitch{15};
        double m_yaw{-30};
        double m_radius{23.0};
        static constexpr double m_minRadius{1.0};
        static constexpr double m_maxRadius{30.0};
        double m_rotationSpeed{0.3};
        double m_zoomSpeed{0.5};

        void updateCameraPosition() const;
    };
}
