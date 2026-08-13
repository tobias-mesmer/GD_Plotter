#include "graphics/Camera.h"
#include "graphics/DebugCube.h"

#include <glm/gtc/type_ptr.hpp>

namespace gdp::graphics {
    DebugCube::DebugCube():m_shader(Shader("../../assets/shaders/debugCube.vs", "../../assets/shaders/debugCube.fs")) {
        init();
    }

    DebugCube::~DebugCube() {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_ebo);
    }

    void DebugCube::init() {
        float vertices[] = {
            -1, -1, -1,
            1, -1, -1,
            1, 1, -1,
            -1, 1, -1,
            -1, -1, 1,
            1, -1, 1,
            1, 1, 1,
            -1, 1, 1
        };

        unsigned int indices[] = {
            // front face
            0, 1, 1, 2, 2, 3, 3, 0,
            // back face
            4, 5, 5, 6, 6, 7, 7, 4,
            // connect
            0, 4, 1, 5, 2, 6, 3, 7
        };

        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);
        glGenBuffers(1, &m_ebo);

        glBindVertexArray(m_vao);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) nullptr);

        glBindVertexArray(0);

        uView = glGetUniformLocation(m_shader.m_program, "uView");
        uProj = glGetUniformLocation(m_shader.m_program, "uProj");
    }

    void DebugCube::draw(const Camera& camera) const {
        glUseProgram(m_shader.m_program);

        glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(camera.viewMatrix()));
        glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(camera.projectionMatrix()));

        glBindVertexArray(m_vao);
        glLineWidth(2);
        glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, nullptr);
        glLineWidth(1);

        // Unbind buffers
        glBindVertexArray(0);
        glUseProgram(0);
    }
}
