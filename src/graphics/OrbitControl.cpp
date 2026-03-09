#include "../../include/graphics/OrbitControl.h"

#include <iostream>
#include <ostream>
#include <GLFW/glfw3.h>

namespace gdp::graphics {
    void OrbitControl::update(Camera &camera, GLFWwindow* window) {
        double x, y;
        glfwGetCursorPos(window, &x, &y);

        if (bool leftDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            const double dx = x - lastX;
            const double dy = y - lastY;

            yaw   += dx * rotationSpeed;
            pitch += dy * rotationSpeed;

            updateCameraPosition();
        }

        lastX = x;
        lastY = y;
    }

    void OrbitControl::updateCameraPosition() {
        std::cout << "OrbitControl::updateCameraPosition " << yaw << "," << pitch << std::endl;
    }

    void OrbitControl::onScroll(double yOffset) {
        std::cout << "OrbitControl::onScroll " << yOffset << std::endl;
    }
}
