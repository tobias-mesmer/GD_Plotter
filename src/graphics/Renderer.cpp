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

#include "plot/TextRenderer.h"
#include "plot/Ticks.h"
#include "graphics/theme.h"

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

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
        glfwSetWindowUserPointer(m_window, this);
        glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
        glfwSetKeyCallback(m_window, key_callback);
        glfwSetScrollCallback(m_window, scroll_callback);
        glfwGetFramebufferSize(m_window, &m_windowWidth, &m_windowHeight);
        updateProjection(m_windowWidth, m_windowHeight);

        {
            // SETUP IMGUI
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
        if (m_windowWidth == 0 || m_windowHeight == 0) return;

        glClearColor(theme::background.color.r, theme::background.color.g, theme::background.color.b, theme::background.color.a);
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

        // Draw FPS GUI
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - m_statsWindowSize.x - 10, 10));
        ImGui::SetNextWindowSize(m_statsWindowSize);
        ImGui::Begin("Stats");
        ImGui::Text("FPS: %.1f", m_fps);
        ImGui::End();

        // Draw tick labels as ImGUI text
        const auto viewProj = m_camera.projectionMatrix() * m_camera.viewMatrix();
        auto color = ImVec4(theme::axis_box.tickLabelsColor.r, theme::axis_box.tickLabelsColor.g, theme::axis_box.tickLabelsColor.b, theme::axis_box.tickLabelsColor.a);

        for (const auto& [labels, edgeStart, edgeEnd, outward]: m_axisBox->ticks().axes) {
            const auto screenEdgeStart = plot::worldToScreen(viewProj, edgeStart);
            const auto screenEdgeEnd = plot::worldToScreen(viewProj, edgeEnd);
            if (!screenEdgeStart || !screenEdgeEnd) continue;

            glm::vec2 edge = {screenEdgeEnd->x - screenEdgeStart->x, screenEdgeEnd->y - screenEdgeStart->y}; // Edge vector in screen space
            const float len = glm::length(edge);

            const float spacing = len / static_cast<float>(labels.size() - 1);
            const float textH = ImGui::GetTextLineHeight();
            const float alpha = glm::smoothstep(textH * 0.2f, textH * 0.9f, spacing); // Start vanishing at 90% overlap-free, vanish fully at under 20%
            if (alpha <= 0) continue;

            const glm::vec3 edgeMid = (edgeStart + edgeEnd) * 0.5f;
            const auto screenMid = plot::worldToScreen(viewProj, edgeMid);
            const auto screenMidOut = plot::worldToScreen(viewProj, edgeMid + outward);
            if (!screenMid || !screenMidOut) continue;

            const glm::vec2 offset{screenMidOut->x - screenMid->x, screenMidOut->y - screenMid->y};
            const float offLen = glm::length(offset);
            const glm::vec2 dir = offLen > 1e-3f ? offset / offLen : glm::vec2(0.0f);
            const float strength = glm::smoothstep(2.0f, 12.0f, offLen);

            color.w = alpha;
            for (const auto& label: labels)
                drawLabel3D(viewProj, label, dir, strength, color);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void Renderer::error_callback(const int error, const char* description) {
        fprintf(stderr, "Error %i: %s\n", error, description);
    }

    void Renderer::framebuffer_size_callback(GLFWwindow* window, const int width, const int height) {
        auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
        renderer->onResize(width, height);
    }

    void Renderer::updateProjection(const int width, const int height) {
        if (width == 0 || height == 0) return;
        m_windowWidth = width;
        m_windowHeight = height;
        glViewport(0, 0, width, height);
        m_camera.setAspectRatio(static_cast<double>(width) / height);
        m_camera.updateProjectionMatrix();
    }

    void Renderer::onResize(const int width, const int height) {
        updateProjection(width, height);
        render(m_camera); // Re-render manually, since callback fires until let go and stalls render loop
    }

    void Renderer::key_callback(GLFWwindow* window, const int key, const int scancode, const int action, const int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    void Renderer::scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
        const auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
        renderer->m_orbitControl.onScroll(yOffset);
    }
}
