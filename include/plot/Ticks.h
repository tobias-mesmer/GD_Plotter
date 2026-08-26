#pragma once
#include <array>
#include <string>
#include <vector>

#include "glm/fwd.hpp"
#include "glm/vec3.hpp"

namespace gdp::plot {
    struct Label {
        std::string text;
        glm::vec3 anchor;
    };

    struct AxisTicks {
        std::vector<Label> labels;
        glm::vec3 edgeStart, edgeEnd; // the edge of the axis the ticks sit on
        glm::vec3 outward; // the outward direction away from the box
    };

    struct TickResult {
        std::vector<glm::vec3> segments;
        std::array<size_t, 3> segmentCountPerAxis;
        std::array<AxisTicks, 3> axes;
    };

    struct Selection {
        std::array<uint8_t, 3> backBit; // encodes per axis whether the back pane sits at min or max
        std::array<uint8_t, 3> edgeCorner; // encodes per axis where the min for the label edge sits

        bool operator==(const Selection& other) const { return this->backBit == other.backBit; }
    };

    enum TickMode { NORMAL, EXTENDED_WILKINSON };

    TickResult ticks(const glm::vec3& min, const glm::vec3& max, unsigned int ticksX, unsigned int ticksY, unsigned int ticksZ, Selection selection, TickMode mode = NORMAL);

    TickResult ticksNormal(const glm::vec3& min, const glm::vec3& max, unsigned int ticksX, unsigned int ticksY, unsigned int ticksZ, Selection selection);

    TickResult extendedWilkinson(const glm::vec3& min, const glm::vec3& max, unsigned int ticksX, unsigned int ticksY, unsigned int ticksZ, Selection selection);

    Selection selectEdges(const glm::vec3& cameraPos, const glm::vec3& center);
}
