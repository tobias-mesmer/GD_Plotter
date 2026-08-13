#include "plot/TextRenderer.h"

#include <optional>

#include "glm/vec4.hpp"
#include "graphics/Camera.h"
#include "plot/Ticks.h"

namespace gdp{
    void plot::drawLabel3D(const glm::mat4& viewProj, const Label& label, const ImVec4& color) {
        const ImVec2 displaySize = ImGui::GetIO().DisplaySize;

        const auto toScreen = [&](const glm::vec3& world)->std::optional<ImVec2> {
            const glm::vec4 clip = viewProj * glm::vec4(world, 1.0f);
            if (clip.w <= 0.0f) return std::nullopt;
            const glm::vec3 ndc = glm::vec3(clip) / clip.w;
            return ImVec2{ (ndc.x * 0.5f + 0.5f) * displaySize.x,
                           (1.0f - (ndc.y * 0.5f + 0.5f)) * displaySize.y };
        };

        const auto screenInner = toScreen(label.inner);
        const auto screenOuter = toScreen(label.outer);
        if (!screenInner || !screenOuter) return;

        ImDrawList* dl = ImGui::GetBackgroundDrawList();
        const ImVec2 size = ImGui::CalcTextSize(label.text.c_str());
        constexpr float padding = 5.0f;
        const glm::vec2 delta{screenOuter->x - screenInner->x, screenOuter->y - screenInner->y};
        const float len = glm::length(delta);
        const glm::vec2 dir = len > 1e-3f ? delta / len : glm::vec2(0.0f);
        const float strength = glm::smoothstep(2.0f, 12.0f, len);
        const float push = 0.5f * (fabsf(dir.x) * size.x + fabsf(dir.y) * size.y) + padding;
        const ImVec2 pos = { screenOuter->x - size.x * 0.5f + dir.x * push * strength,
                     screenOuter->y - size.y * 0.5f + dir.y * push * strength };

        dl->AddText(pos, ImGui::ColorConvertFloat4ToU32(color), label.text.c_str());
    }
}
