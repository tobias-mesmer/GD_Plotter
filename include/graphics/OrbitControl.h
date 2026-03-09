#pragma once

#include "Camera.h"
#include <GLFW/glfw3.h>

namespace gdp::graphics {
    class OrbitControl {
    public:
        void update(Camera& camera, GLFWwindow* window);
        void onScroll(double yOffset);
    private:
        double lastX{0};
        double lastY{0};
        double pitch{0};
        double yaw{0};
        double rotationSpeed{0};

        void updateCameraPosition();
    };
}
