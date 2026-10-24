#pragma once

#include "Ticks.h"
#include "glm/vec3.hpp"

namespace gdp::plot::Box {
    /**
     * Computes the corresponding corner vec3 from a given unit cube index.
     * @param min The minimal extents of the axis box.
     * @param max The maximal extents of the axis box.
     * @param index The index of which of the 8 corners to be computed in [0, ..., 7].
     * @return The corresponding vec3.
     */
    constexpr glm::vec3 cornerVec(const glm::vec3& min, const glm::vec3& max, const unsigned index) {
        return {
            index & 1 ? max.x : min.x,
            index & 2 ? max.y : min.y,
            index & 4 ? max.z : min.z
        };
    }

    // Unit cube index of the corner with bit aBit on axis a and placement j (0–3) on the other two axes
    constexpr unsigned cornerIndex(const unsigned a, const unsigned aBit, const unsigned j) {
        // Calculate bit positions of the other axes -> edges along axis a only vary in b and c
        const unsigned b = (a + 1) % 3; // axis b
        const unsigned c = (a + 2) % 3; // axis c

        // Which of the 4 combinations depending on j
        const unsigned offset =
                (aBit << a) |
                ((j & 1) << b) | // toggle applying b
                ((j >> 1) << c); // apply c for the second pair

        return offset;
    }

    /**
     * Computes the unit cube indices for start and end corners/vertices for an edge with index e in [0, ..., 11].
     * @param e The edge index, defined as: e = a * 4 + j, for axis a and offset j
     * @return Start and end unit cube indices of the edge e.
     */
    constexpr std::array<unsigned, 2> edgeCornersIndices(const unsigned e) {
        const unsigned a = e / 4; // which of the 3 axes
        const unsigned j = e % 4; // which of the 4 placement options for the current axis

        const unsigned start = cornerIndex(a, 0, j); // a bit = 0 -> min[a]
        const unsigned end = cornerIndex(a, 1, j); // a bit = 1 -> max[a]

        return {start, end};
    }

    constexpr float back(const int axis, const glm::vec3& min, const glm::vec3& max, const Selection& selection) { return selection.backBit[axis] ? max[axis] : min[axis]; }

    constexpr float front(const int axis, const glm::vec3& min, const glm::vec3& max, const Selection& selection) { return selection.backBit[axis] ? min[axis] : max[axis]; }
}
