#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "glad.h"
#include "graphics/Renderer.h"

using namespace gdp::graphics;

int main() {
    std::cout << "========== GD Plotter ==========" << std::endl;

    const auto renderer = std::make_shared<Renderer>();
    const auto window = renderer->getWindow();

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0, 0.2, 0.3, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    exit(EXIT_SUCCESS);
}