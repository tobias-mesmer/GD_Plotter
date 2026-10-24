#include "plot/Grid.h"

#include "plot/Box.h"

// Generate grid line segments from selected back panes by starting from where they intersect, then computing the front per axis.
std::vector<glm::vec3> gdp::plot::grid(const glm::vec3& min, const glm::vec3& max, const TickResult& ticks, Selection selection) {
    std::vector<glm::vec3> result;

    for (int a = 0; a < 3; ++a) {

        // The other axes
        const int b = (a + 1) % 3;
        const int c = (a + 2) % 3;

        for (size_t i = 0; i < ticks.tickCountPerAxis[a]; ++i) {
            auto tick = ticks.getTickAt(a, i);

            // Construct the tick pos that lies on the line where two back panes intersect
            glm::vec3 backTick{};
            backTick[a] = tick[a];
            backTick[b] = Box::back(b, min, max, selection);
            backTick[c] = Box::back(c, min, max, selection);

            // Flip individual other axes to get front
            result.push_back(backTick);
            auto frontTickB = backTick;
            frontTickB[b] = Box::front(b, min, max, selection);
            result.push_back(frontTickB);

            result.push_back(backTick);
            auto frontTickC = backTick;
            frontTickC[c] = Box::front(c, min, max, selection);
            result.push_back(frontTickC);
        }
    }
    return result;
}
