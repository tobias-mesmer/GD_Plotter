#include "../../include/graphics/AxisBox.h"

#include <glm/gtc/type_ptr.hpp>

namespace gdp::graphics {
    AxisBox::AxisBox() : m_shader(Shader("../../assets/shaders/axisBox.vs", "../../assets/shaders/axisBox.fs")) {
        init();
    }

    AxisBox::~AxisBox() {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_ebo);
    }

    void AxisBox::init() {
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
        uModel = glGetUniformLocation(m_shader.m_program, "uModel");

        setExtents(m_extentsMin, m_extentsMax); // Apply extents to Model matrix
    }

    void AxisBox::draw(const Camera& camera) const {
        glUseProgram(m_shader.m_program);

        glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
        glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(camera.getProjectionMatrix()));
        glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(m_model));

        glBindVertexArray(m_vao);
        glLineWidth(1);
        glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, nullptr);

        // Unbind buffers
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void AxisBox::setExtents(const glm::vec3& min, const glm::vec3& max) {
        glm::vec3 center = (min + max) * 0.5f;
        glm::vec3 size = (max - min) * 0.5f;

        m_model = glm::translate(glm::mat4(1.0f), center)
                  * glm::scale(glm::mat4(1.0f), size);
    }
}
