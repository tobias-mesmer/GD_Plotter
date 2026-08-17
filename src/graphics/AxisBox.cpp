#include "../../include/graphics/AxisBox.h"
#include "graphics/AxisBox.h"

#include "plot/Ticks.h"
#include <glm/gtc/type_ptr.hpp>

#include "graphics/theme.h"

namespace gdp::graphics {
    AxisBox::AxisBox() : m_boxStyle(BACK), m_shader(Shader("../../assets/shaders/axisBox.vs", "../../assets/shaders/axisBox.fs")) {
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

        // The combination of a vertex' signs in this representation automatically corresponds to its index:
        // vertex       binary      index
        // -1, -1, -1   000         0
        // 1, -1, -1    001         1
        // -1, 1, -1    010         2
        // 1, 1, -1     011         3
        // ...

        float vertices[] = {
            -1, -1, -1,
            1, -1, -1,
            -1, 1, -1,
            1, 1, -1,
            -1, -1, 1,
            1, -1, 1,
            -1, 1, 1,
            1, 1, 1
        };

        constexpr unsigned int numEdges = 12 * 2;
        constexpr unsigned int numPanes = 6 * 2 * 3;
        unsigned int indices[numEdges + numPanes]; // 12 edges with 2 verts each for edge rendering, then 6 faces with 2 tris each for pane rendering

        // Calculate edge and pane indices per axis
        for (unsigned int i = 0; i < 3; i++) {
            // Calculate bit positions of the axes, e.g.:
            //      cba   or    bac
            //      110         101
            //      zyx         zyx
            const unsigned a = i;
            const unsigned b = (a + 1) % 3;
            const unsigned c = (a + 2) % 3;

            for (unsigned int j = 0; j < 4; ++j) {
                // Go through the 4 combinations
                const unsigned B = (j & 1) << b; // toggle applying b
                const unsigned C = (j >> 1) << c; // apply c for the second pair

                // Edges
                const unsigned start = B | C;
                const unsigned end = start | (1 << a);
                indices[(a * 4 + j) * 2] = start;
                indices[(a * 4 + j) * 2 + 1] = end;
            }
        }

        // Create pane tris
        for (int i = 0; i < 6; ++i) {
            // First tri
            indices[numEdges + i * 6] = indices[i * 4];
            indices[numEdges + i * 6 + 1] = indices[i * 4 + 1];
            indices[numEdges + i * 6 + 2] = indices[i * 4 + 2];
            // Second tri
            indices[numEdges + i * 6 + 3] = indices[i * 4 + 1];
            indices[numEdges + i * 6 + 4] = indices[i * 4 + 2];
            indices[numEdges + i * 6 + 5] = indices[i * 4 + 3];
        }

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

    void AxisBox::update(const Camera& camera) {
        const plot::EdgeSelection sel = plot::selectEdges(camera.viewDirection(), m_extentsMin, m_extentsMax);
        if (sel == m_currentSelection) return;
        m_currentSelection = sel;
        //rebuildTicks();
    }

    void AxisBox::draw(const Camera& camera) {
        glUseProgram(m_shader.m_program);
        glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(camera.viewMatrix()));
        glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(camera.projectionMatrix()));

        switch (m_boxStyle) {
            case FULL:
                drawWireFrame();
                drawTickLines();
                break;
            case BACK:
                //update(camera);
                drawBackFacing();
                drawTicksFront();
                break;
        }

        glUseProgram(0);
    }

    void AxisBox::setRenderMode(BoxStyle mode) {
        m_boxStyle = mode;
    }

    void AxisBox::drawWireFrame() const {
        glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(m_model));
        glUniform4fv(uColor, 1, glm::value_ptr(theme::axis_box.boxColor));

        glBindVertexArray(m_vao);

        glLineWidth(1);
        glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);
    }

    void AxisBox::drawBackFacing() const {
        glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(m_model));
        glm::vec4 color = theme::axis_box.boxColor;
        color.a = 0.2f;
        glUniform4fv(uColor, 1, glm::value_ptr(color));

        glBindVertexArray(m_vao);

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, reinterpret_cast<void*>(24 * sizeof(int)));

        glBindVertexArray(0);
    }

    void AxisBox::drawTickLines() const {
        glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

        glBindVertexArray(m_vao_ticks);

        glLineWidth(2);

        glUniform4fv(uColor, 1, glm::value_ptr(theme::axis_box.ticksColorX));
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(2 * m_ticksX));
        glUniform4fv(uColor, 1, glm::value_ptr(theme::axis_box.ticksColorY));
        glDrawArrays(GL_LINES, static_cast<GLsizei>(2 * m_ticksX), static_cast<GLsizei>(2 * m_ticksY));
        glUniform4fv(uColor, 1, glm::value_ptr(theme::axis_box.ticksColorZ));
        glDrawArrays(GL_LINES, static_cast<GLsizei>(2 * (m_ticksX + m_ticksY)), static_cast<GLsizei>(2 * m_ticksZ));

        glBindVertexArray(0);
    }

    void AxisBox::drawTicksFront() const {
        // TODO
    }

    void AxisBox::setExtents(const glm::vec3& min, const glm::vec3& max) {
        m_extentsMin = min;
        m_extentsMax = max;

        const glm::vec3 center = (min + max) * 0.5f;
        const glm::vec3 size = (max - min) * 0.5f;

        m_model = glm::translate(glm::mat4(1.0f), center)
                  * glm::scale(glm::mat4(1.0f), size);

        m_ticks = plot::ticks(min, max, m_ticksX, m_ticksY, m_ticksZ, plot::NORMAL);

        glBindVertexArray(m_vao_ticks);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_ticks);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizei>(sizeof(glm::vec3) * m_ticks.segments.size()), m_ticks.segments.data(), GL_DYNAMIC_DRAW);

        glBindVertexArray(0);
    }
}
