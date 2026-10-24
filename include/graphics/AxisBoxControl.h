#pragma once
#include "AxisBox.h"
#include "Camera.h"

#include <GLFW/glfw3.h>

namespace gdp::graphics {
    class AxisBoxControl {
    public:
        explicit AxisBoxControl(AxisBox& axisBox) : m_axisBox(axisBox) {
        }

        void update(GLFWwindow* window, const Camera& camera);

        [[nodiscard]] bool isDragging() const { return m_isDragging; }

    private:
        gdp::graphics::AxisBox& m_axisBox;
        bool m_isDragging{false};
    };
}
