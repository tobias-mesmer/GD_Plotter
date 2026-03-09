#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "Theme.h"

namespace gdp::graphics {
    class Renderer {
    public:
        explicit Renderer(Camera &camera);

        ~Renderer();

        [[nodiscard]] GLFWwindow* getWindow() const {
            return m_window;
        }

        void render(const Camera &camera) const;

    private:
        GLFWwindow* m_window;
        Camera &camera;
        Theme m_theme;

        static void error_callback(int error, const char* description);

        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    };


}
