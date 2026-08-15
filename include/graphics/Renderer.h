#pragma once
#define GLFW_INCLUDE_NONE

#include <memory>

#include "AxisBox.h"
#include "Camera.h"
#include "DebugCube.h"
#include "imgui.h"
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
        std::unique_ptr<AxisBox> m_axisBox;
        std::unique_ptr<DebugCube> m_debugCube;

        int m_windowWidth{1280};
        int m_windowHeight{720};

        unsigned int m_frameCounter{0};
        float m_fps{0.0};
        float m_lastFpsTime{0.0};
        float m_elapsed{0.0};

        const ImVec2 m_statsWindowSize{100, 50};

        static void error_callback(int error, const char* description);

        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

        void updateProjection(int width, int height);

        void onResize(int width, int height);

        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

        static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

        void drawGUI() const;

        void updateFPS();
    };
}
