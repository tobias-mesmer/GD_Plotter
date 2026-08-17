#pragma once
#include <array>
#include <string>
#include <vector>
#include "glm/vec3.hpp"

namespace gdp::plot {
    struct Label {
        std::string text;
        glm::vec3 anchor;
    };

    struct AxisTicks {
        std::vector<Label> labels;
        glm::vec3 edgeStart, edgeEnd; // the edge of the axis the ticks sit on, for screen-space fir
        glm::vec3 outward;
    };

    struct TickResult {
        std::vector<glm::vec3> segments;
        std::array<size_t, 3>  segmentCountPerAxis;
        std::array<AxisTicks, 3> axes;
    };

    struct EdgeSelection {
        std::array<uint8_t,3> backBit;

        bool operator==(const EdgeSelection& other) const {return this->backBit == other.backBit;};
    };

    enum TickMode {NORMAL, EXTENDED_WILKINSON};

    TickResult ticks(const glm::vec3& min, const glm::vec3& max, unsigned int ticksX, unsigned int ticksY, unsigned int ticksZ, TickMode mode = NORMAL);

    TickResult ticksNormal(const glm::vec3& min, const glm::vec3& max, unsigned int ticksX, unsigned int ticksY, unsigned int ticksZ);

    TickResult extendedWilkinson(const glm::vec3& min, const glm::vec3& max, unsigned int ticksX, unsigned int ticksY, unsigned int ticksZ);

    EdgeSelection selectEdges(const glm::vec3& cameraViewDir, const glm::vec3& min, const glm::vec3& max);
}
