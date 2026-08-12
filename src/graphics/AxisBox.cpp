#include "../../include/graphics/AxisBox.h"

#include "plot/Ticks.h"
#include <glm/gtc/type_ptr.hpp>

namespace gdp::graphics {
    AxisBox::AxisBox() : m_shader(Shader("../../assets/shaders/axisBox.vs", "../../assets/shaders/axisBox.fs")) {
        init();
    }

    AxisBox::~AxisBox() {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_ebo);

        glDeleteVertexArrays(1, &m_vao_ticks);
        glDeleteBuffers(1, &m_vbo_ticks);
    }

    void AxisBox::init() {
        // ========== AxisBox ==========
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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));

        glBindVertexArray(0);

        uView = glGetUniformLocation(m_shader.m_program, "uView");
        uProj = glGetUniformLocation(m_shader.m_program, "uProj");
        uModel = glGetUniformLocation(m_shader.m_program, "uModel");
        uColor = glGetUniformLocation(m_shader.m_program, "uColor");

        // ========== Ticks ==========

        glGenVertexArrays(1, &m_vao_ticks);
        glGenBuffers(1, &m_vbo_ticks);

        glBindVertexArray(m_vao_ticks);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_ticks);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));

        glBindVertexArray(0);

        setExtents(m_extentsMin, m_extentsMax); // Apply extents to Model matrix and calculate ticks
    }

    void AxisBox::draw(const Camera& camera) const {
        glUseProgram(m_shader.m_program);
        glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
        glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(camera.getProjectionMatrix()));

        drawBox();
        drawTicks();

        glUseProgram(0);
    }

    void AxisBox::drawBox() const {
        glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(m_model));
        glUniform4fv(uColor, 1, glm::value_ptr(glm::vec4(glm::vec3(0.7), 1.0))); // grey

        glBindVertexArray(m_vao);

        glLineWidth(1);
        glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);
    }

    void AxisBox::drawTicks() const {
        glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

        glBindVertexArray(m_vao_ticks);

        glLineWidth(2);

        glUniform4fv(uColor, 1, glm::value_ptr(glm::vec4(glm::vec3(0.84, 0.24, 0.30), 1.0)));
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(2 * m_ticksX));
        glUniform4fv(uColor, 1, glm::value_ptr(glm::vec4(glm::vec3(0.55, 0.75, 0.25), 1.0)));
        glDrawArrays(GL_LINES, static_cast<GLsizei>(2 * m_ticksX), static_cast<GLsizei>(2 * m_ticksY));
        glUniform4fv(uColor, 1, glm::value_ptr(glm::vec4(glm::vec3(0.25, 0.45, 0.85), 1.0)));
        glDrawArrays(GL_LINES, static_cast<GLsizei>(2 * (m_ticksX + m_ticksY)), static_cast<GLsizei>(2 * m_ticksZ));

        glBindVertexArray(0);
    }

    void AxisBox::setExtents(const glm::vec3& min, const glm::vec3& max) {
        m_extentsMin = min;
        m_extentsMax = max;

        glm::vec3 center = (min + max) * 0.5f;
        glm::vec3 size = (max - min) * 0.5f;

        m_model = glm::translate(glm::mat4(1.0f), center)
                  * glm::scale(glm::mat4(1.0f), size);

        m_ticks = plot::ticks(min, max, m_ticksX, m_ticksY, m_ticksZ);

        glBindVertexArray(m_vao_ticks);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_ticks);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizei>(sizeof(glm::vec3) * m_ticks.size()), m_ticks.data(), GL_DYNAMIC_DRAW);

        glBindVertexArray(0);
    }
}
