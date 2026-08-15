#pragma once
#include <imgui.h>
#include <optional>
#include <string>

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

namespace gdp::graphics {
    class Camera;
}

namespace gdp::plot {
    struct Label;

    void drawLabel3D(const glm::mat4& viewProj, const Label& label, const glm::vec2& dir, float strength, const ImVec4& color = ImVec4(1.0, 1.0, 1.0, 1.0));

    std::optional<ImVec2> worldToScreen(const glm::mat4& viewProj, const glm::vec3& world);
}
