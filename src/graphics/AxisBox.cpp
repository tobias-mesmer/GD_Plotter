#include "graphics/AxisBox.h"

#include <iostream>

#include "plot/Ticks.h"
#include <glm/gtc/type_ptr.hpp>

#include "graphics/theme.h"

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

        // The combination of a vertex' signs in this representation automatically corresponds to its index:
        // vertex       binary      index
        // -1, -1, -1   000         0
        // 1, -1, -1    001         1
        // -1, 1, -1    010         2
        // 1, 1, -1     011         3
        // ...

        constexpr float vertices[] = {
            -1, -1, -1,
            1, -1, -1,
            -1, 1, -1,
            1, 1, -1,
            -1, -1, 1,
            1, -1, 1,
            -1, 1, 1,
            1, 1, 1
        };

        constexpr unsigned int numEdgeIndices = 12 * 2;
        constexpr unsigned int numPaneIndices = 6 * 2 * 3;
        unsigned int indices[numEdgeIndices + numPaneIndices]; // 12 edges with 2 verts each for edge rendering, then 6 faces with 2 tris each for pane rendering

        // Calculate edge and pane indices per axis a
        for (unsigned int a = 0; a < 3; a++) {
            // Calculate bit positions of the other axes -> edges along axis a only vary in b and c
            const unsigned b = (a + 1) % 3;
            const unsigned c = (a + 2) % 3;

            auto corner = [&](const unsigned int j, const unsigned aBit) {
                // Go through the 4 combinations depending on j
                return (aBit << a) |
                       ((j & 1) << b) | // toggle applying b
                       ((j >> 1) << c); // apply c for the second pair
            };

            // Edges in direction of axis a (start at a=0, end at a=1)
            for (unsigned int j = 0; j < 4; ++j) {
                indices[(a * 4 + j) * 2] = corner(j, 0);
                indices[(a * 4 + j) * 2 + 1] = corner(j, 1);
            }

            // Create tris for the 2 panes perpendicular to axis a
            constexpr unsigned int pattern[6] = {0, 1, 3, 0, 3, 2};
            for (unsigned int side = 0; side < 2; ++side) {
                for (unsigned int p = 0; p < 6; ++p) {
                    indices[numEdgeIndices + (a * 2 + side) * 6 + p] = corner(pattern[p], side);
                }
            }
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

    void AxisBox::updateSelection(const Camera& camera) {
        const plot::Selection sel = plot::selectEdges(camera.position(), center());
        const bool dirty = !m_currentSelection || (m_boxStyle == BACK && sel != *m_currentSelection);
        if (!dirty) return;
        m_currentSelection = sel;
        //std::cout << "Camera octant: [" << (sel.backBit[0] == 0 ? '-' : '+') << "," << (sel.backBit[1] == 0 ? '-' : '+') << "," << (sel.backBit[2] == 0 ? '-' : '+') << "]" << std::endl;
        rebuildTicks();
    }

    void AxisBox::rebuildTicks() {
        m_ticks = plot::ticks(m_extentsMin, m_extentsMax, m_ticksX, m_ticksY, m_ticksZ, m_boxStyle == BACK ? *m_currentSelection : plot::Selection{}, m_tickMode);

        glBindVertexArray(m_vao_ticks);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_ticks);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizei>(sizeof(glm::vec3) * m_ticks.segments.size()), m_ticks.segments.data(), GL_DYNAMIC_DRAW);

        glBindVertexArray(0);
    }

    void AxisBox::draw(const Camera& camera) {
        glUseProgram(m_shader.m_program);
        glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(camera.viewMatrix()));
        glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(camera.projectionMatrix()));

        updateSelection(camera);

        switch (m_boxStyle) {
            case FULL:
                drawWireFrame();
                break;
            case BACK:
                drawBackFacing();
                break;
        }

        drawTickLines();

        glUseProgram(0);
    }

    void AxisBox::setRenderMode(BoxStyle mode) {
        m_boxStyle = mode;
        m_currentSelection.reset();
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
        glUniform4fv(uColor, 1, glm::value_ptr(theme::axis_box.paneColor));

        glBindVertexArray(m_vao);

        for (int a = 0; a < 3; ++a) {
            const int side = m_currentSelection->backBit[a];
            const int slot = a * 2 + side;
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, reinterpret_cast<void*>((24 + slot * 6) * sizeof(unsigned)));
        }

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

    void AxisBox::setExtents(const glm::vec3& min, const glm::vec3& max) {
        m_extentsMin = min;
        m_extentsMax = max;

        const glm::vec3 center = (min + max) * 0.5f;
        const glm::vec3 size = (max - min) * 0.5f;

        m_model = glm::translate(glm::mat4(1.0f), center)
                  * glm::scale(glm::mat4(1.0f), size);

        m_currentSelection.reset();
    }
}
