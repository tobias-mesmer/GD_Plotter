#pragma once
#include <imgui.h>
#include <string>

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

namespace gdp::graphics {
    class Camera;
}

namespace gdp::plot {
    struct Label;

    void drawLabel3D(const glm::mat4& viewProj, const Label& label, const ImVec4& color = ImVec4(1.0, 1.0, 1.0, 1.0));
}
