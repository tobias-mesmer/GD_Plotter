#include "../../include/graphics/Renderer.h"
#include <iostream>
#include "glad.h"

namespace gdp::graphics {

    Renderer::Renderer() {
        glfwSetErrorCallback(error_callback);

        if (!glfwInit()) exit(EXIT_FAILURE);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

        window = glfwCreateWindow(640, 480, "GD_Plotter", nullptr, nullptr);
        if (!window)
        {
            glfwTerminate();
            exit(EXIT_FAILURE);
        }

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
            std::cerr << "Failed to initialize GLAD\n";
            exit(EXIT_FAILURE);
        }

        glfwSetKeyCallback(window, key_callback);
    }

    Renderer::~Renderer() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void Renderer::error_callback(int error, const char* description)
    {
        fprintf(stderr, "Error: %s\n", description);
    }

    void Renderer::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

}
