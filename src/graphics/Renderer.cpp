#include "../../include/graphics/Renderer.h"
#include <iostream>
#include "glad.h"
#include "graphics/OrbitControl.h"

namespace gdp::graphics {
    Renderer::Renderer(Camera& camera) : m_camera(camera) {
        glfwSetErrorCallback(error_callback);

        if (!glfwInit()) exit(EXIT_FAILURE);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

        m_window = glfwCreateWindow(m_windowWidth, m_windowHeight, "GD_Plotter", nullptr, nullptr);
        if (!m_window) {
            glfwTerminate();
            exit(EXIT_FAILURE);
        }

        glfwMakeContextCurrent(m_window);
        glfwSwapInterval(1);

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
            std::cerr << "Failed to initialize GLAD\n";
            exit(EXIT_FAILURE);
        }

        glfwSetWindowUserPointer(m_window, this);
        glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
        glfwSetKeyCallback(m_window, key_callback);
        glfwSetScrollCallback(m_window, scroll_callback);
    }

    Renderer::~Renderer() {
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
    }

    void Renderer::error_callback(int error, const char* description) {
        fprintf(stderr, "Error: %s\n", description);
    }

    void Renderer::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
        renderer->onResize(width, height);
    }

    void Renderer::onResize(int width, int height) {
        m_windowWidth = width;
        m_windowHeight = height;

        m_camera.setAspectRatio(static_cast<double>(width) / height);
        m_camera.updateProjectionMatrix();
    }


    void Renderer::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    void Renderer::scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
        auto* control = static_cast<OrbitControl*>(glfwGetWindowUserPointer(window));
        control->onScroll(yOffset);
    }
}
