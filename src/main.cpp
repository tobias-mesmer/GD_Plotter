#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "graphics/OrbitControl.h"
#include "graphics/AxisBoxControl.h"
#include "graphics/Renderer.h"

using namespace gdp::graphics;

int main() {
    std::cout << "==================== GD Plotter ====================\n\n" <<
            "GD Plotter Copyright (C) 2026 Tobias Mesmer.\n" <<
            "This program comes with ABSOLUTELY NO WARRANTY;\n" <<
            "This is free software, and you are welcome to redistribute it\n" <<
            "under certain conditions. See LICENSE.md.\n\n" <<
            "====================================================\n" << std::flush;

    Camera camera{};
    OrbitControl orbitControl{camera};
    Renderer renderer{camera, orbitControl};
    AxisBoxControl axisBoxControl{renderer.axisBox()};
    auto* const window = renderer.getWindow();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        axisBoxControl.update(window, camera);
        if (!axisBoxControl.isDragging()) orbitControl.update(window);
        renderer.render(camera);
    }

    return EXIT_SUCCESS;
}
