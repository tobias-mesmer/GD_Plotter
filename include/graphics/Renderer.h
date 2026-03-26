#pragma once
#define GLFW_INCLUDE_NONE

#include <memory>

#include "Camera.h"
#include "GridRenderer.h"
#include "OrbitControl.h"
#include "Theme.h"

namespace gdp::graphics {
    class Renderer {
    public:
        explicit Renderer(Camera& camera, OrbitControl& orbitControl);

        ~Renderer();

        [[nodiscard]] GLFWwindow* getWindow() const {
            return m_window;
        }

        void render(const Camera& camera);

    private:
        GLFWwindow* m_window;
        Camera& m_camera;
        OrbitControl& m_orbitControl;
        std::unique_ptr<GridRenderer> m_gridRenderer;
        Theme m_theme;

        int m_windowWidth{1280};
        int m_windowHeight{720};

        static void error_callback(int error, const char* description);

        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

        void onResize(int width, int height);

        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

        static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    };
}
