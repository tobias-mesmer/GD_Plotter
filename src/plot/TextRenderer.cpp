#include "plot/TextRenderer.h"

#include <optional>

#include "glm/vec4.hpp"
#include "graphics/Camera.h"
#include "plot/Ticks.h"

namespace gdp{

    // Draw tick labels as ImGUI text, located at anchor point and pushed outwards in screen space by the given direction.
    void plot::drawLabel3D(const glm::mat4& viewProj, const Label& label, const glm::vec2& dir, const float strength, const ImVec4& color) {
        ImDrawList* dl = ImGui::GetBackgroundDrawList();
        constexpr float padding = 5.0f;
        const auto screenAnchor = worldToScreen(viewProj, label.anchor);
        if (!screenAnchor) return;
        const ImVec2 size = ImGui::CalcTextSize(label.text.c_str());
        const float push = 0.5f * (fabsf(dir.x) * size.x + fabsf(dir.y) * size.y) + padding;
        const ImVec2 pos = { screenAnchor->x - size.x * 0.5f + dir.x * push * strength,
                             screenAnchor->y - size.y * 0.5f + dir.y * push * strength };

        dl->AddText(pos, ImGui::ColorConvertFloat4ToU32(color), label.text.c_str());
    }

    // Project world space to screen space
    std::optional<ImVec2> plot::worldToScreen(const glm::mat4& viewProj, const glm::vec3& world) {
        const ImVec2 displaySize = ImGui::GetIO().DisplaySize;
        const glm::vec4 clip = viewProj * glm::vec4(world, 1.0f);
        if (clip.w <= 0.0f) return std::nullopt;
        const glm::vec3 ndc = glm::vec3(clip) / clip.w;
        return ImVec2{ (ndc.x * 0.5f + 0.5f) * displaySize.x,
                       (1.0f - (ndc.y * 0.5f + 0.5f)) * displaySize.y };
    }
}
