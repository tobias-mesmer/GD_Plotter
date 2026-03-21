#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "graphics/OrbitControl.h"
#include "graphics/Renderer.h"

using namespace gdp::graphics;

int main() {
    std::cout << "==================== GD Plotter ====================" << std::endl;
    std::cout << std::endl <<
            "GD Plotter Copyright (C) 2026 Tobias Mesmer." << std::endl <<
            "This program comes with ABSOLUTELY NO WARRANTY;" << std::endl <<
            "This is free software, and you are welcome to redistribute it" << std::endl <<
            "under certain conditions. See LICENSE.md." << std::endl
            << std::endl;
    std::cout << "====================================================" << std::endl;

    Camera camera{};
    OrbitControl control{camera};
    Renderer renderer{camera};
    const auto window = renderer.getWindow();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        control.update(window);
        renderer.render(camera);

        glfwSwapBuffers(window);
    }

    exit(EXIT_SUCCESS);
}
