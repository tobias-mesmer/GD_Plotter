#include "graphics/Renderer.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "graphics/DebugCube.h"
#include "graphics/OrbitControl.h"

#include <iostream>
#include <sstream>

namespace gdp::graphics {
    Renderer::Renderer(Camera& camera, OrbitControl& orbitControl) : m_camera(camera), m_orbitControl(orbitControl) {
        glfwSetErrorCallback(error_callback);

        if (!glfwInit()) exit(EXIT_FAILURE);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 4);

        m_window = glfwCreateWindow(m_windowWidth, m_windowHeight, "GD_Plotter", nullptr, nullptr);
        if (!m_window) {
            glfwTerminate();
            exit(EXIT_FAILURE);
        }

        glfwMakeContextCurrent(m_window);
        glfwSwapInterval(0);

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
            std::cerr << "Failed to initialize GLAD\n";
            exit(EXIT_FAILURE);
        }

        glEnable(GL_MULTISAMPLE);
        glfwSetWindowUserPointer(m_window, this);
        glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
        glfwSetKeyCallback(m_window, key_callback);
        glfwSetScrollCallback(m_window, scroll_callback);

        { // SETUP IMGUI
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            ImGui_ImplGlfw_InitForOpenGL(m_window, true);
            ImGui_ImplOpenGL3_Init("#version 450");
            ImGui::StyleColorsDark();
        }

        m_axisBox = std::make_unique<AxisBox>();
        m_debugCube = std::make_unique<DebugCube>();
    }

    Renderer::~Renderer() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    void Renderer::render(const Camera& camera) {
        glfwGetFramebufferSize(m_window, &m_windowWidth, &m_windowHeight);

        m_camera.setAspectRatio(static_cast<double>(m_windowWidth) / m_windowHeight);
        m_camera.updateProjectionMatrix();

        glClearColor(m_theme.background.color.r, m_theme.background.color.g, m_theme.background.color.b, m_theme.background.color.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render
        m_debugCube->draw(camera);
        m_axisBox->draw(camera);

        updateFPS();
        drawGUI();

        glfwSwapBuffers(m_window);
    }

    void Renderer::updateFPS() {
        m_frameCounter++;
        const auto now = glfwGetTime();
        m_elapsed = now - m_lastFpsTime;
        if (m_elapsed >= 1.0) {
            m_fps = m_frameCounter / m_elapsed;
            m_frameCounter = 0;
            m_lastFpsTime = now;
        }
    }

    void Renderer::drawGUI() const {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - m_statsWindowSize.x - 10, 10));
        ImGui::SetNextWindowSize(m_statsWindowSize);
        ImGui::Begin("Stats");
        ImGui::Text("FPS: %.1f", m_fps);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void Renderer::error_callback(int error, const char* description) {
        fprintf(stderr, "Error %i: %s\n", error, description);
    }

    void Renderer::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
        renderer->onResize(width, height);
    }

    void Renderer::onResize(int width, int height) {
        glViewport(0, 0, width, height); // Adjust viewport accordingly
        render(m_camera); // Re-render manually, since callback fires until let go and stalls render loop
    }


    void Renderer::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    void Renderer::scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
        auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
        renderer->m_orbitControl.onScroll(yOffset);
    }
}
