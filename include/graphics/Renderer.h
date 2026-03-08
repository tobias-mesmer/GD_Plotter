#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace gdp::graphics {
    class Renderer {
    public:
        Renderer();

        ~Renderer();

        [[nodiscard]] GLFWwindow* getWindow() const {
            return window;
        }

    private:
        GLFWwindow* window;

        static void error_callback(int error, const char* description);

        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    };


}
