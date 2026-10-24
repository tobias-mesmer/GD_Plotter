#include "graphics/AxisBoxControl.h"

#include <GLFW/glfw3.h>

#include <algorithm>
#include <glm/geometric.hpp>

#include "plot/Box.h"
#include "plot/TextRenderer.h"

namespace {
    bool isOnBackPane(const unsigned e, const gdp::plot::Selection& selection) {
        const unsigned a = e / 4; // which of the 3 axes
        const unsigned j = e % 4; // which of the 4 placement options for the current axis

        const unsigned b = (a + 1) % 3; // axis b
        const unsigned c = (a + 2) % 3; // axis c

        // Edge e along axis a is on a back pane if its fixed side on axis b or c matches that axis' back side
        return (j & 1) == selection.backBit[b] || (j >> 1) == selection.backBit[c];
    }
}

void gdp::graphics::AxisBoxControl::update(GLFWwindow* window, const Camera& camera) {
    auto sel = m_axisBox.selection();
    if (!sel.has_value()) return;

    double x, y;
    glfwGetCursorPos(window, &x, &y);
    const glm::vec2 mousePos{x, y};

    const auto viewProj = camera.projectionMatrix() * camera.viewMatrix();

    constexpr float threshold = 8.f;
    std::optional<unsigned> bestEdge;
    float bestDistance = threshold;

    for (unsigned e = 0; e < 12; ++e) {
        if (!isOnBackPane(e, sel.value())) continue;

        const auto [startIdx, endIdx] = plot::Box::edgeCornersIndices(e);
        const auto start3D = plot::Box::cornerVec(m_axisBox.min(), m_axisBox.max(), startIdx);
        const auto end3D = plot::Box::cornerVec(m_axisBox.min(), m_axisBox.max(), endIdx);

        const auto start2Dimvec = plot::worldToScreen(viewProj, start3D);
        const auto end2Dimvec = plot::worldToScreen(viewProj, end3D);

        if (!start2Dimvec.has_value() || !end2Dimvec.has_value()) continue;

        const glm::vec2 start2D{start2Dimvec.value().x, start2Dimvec.value().y};
        const glm::vec2 end2D{end2Dimvec.value().x, end2Dimvec.value().y};
        const glm::vec2 edge2D = end2D - start2D;

        auto len = glm::dot(edge2D, edge2D);
        // Edges shorter than a pixel are a point on screen, so measure the distance to their start
        float t = 0.f;
        if (len >= 1.f) t = std::clamp(glm::dot(mousePos - start2D, edge2D) / len, 0.f, 1.f);
        auto projected = start2D + t * edge2D;

        if (auto distance = glm::length(projected - mousePos); distance < threshold && distance < bestDistance) {
            bestDistance = distance;
            bestEdge = e;
        }
    }
    m_axisBox.setHighlightedEdge(bestEdge);
}
