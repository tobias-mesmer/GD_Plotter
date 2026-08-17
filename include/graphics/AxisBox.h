#pragma once

#include <glad/glad.h>

#include "Shader.h"
#include "graphics/Camera.h"
#include "plot/Ticks.h"


namespace gdp::graphics {
    class AxisBox {
    public:
        AxisBox();

        ~AxisBox();

        // Full: Whole AxisBox is drawn with ticks on the actual XYZ-axes, facing away from the box at 45°
        // Back: Only back-facing part of the box is drawn with respect to the camera, with grid fill and ticks and labels at the front-facing axes
        enum BoxStyle { FULL, BACK };

        void draw(const Camera& camera);

        void setRenderMode(BoxStyle mode);

        void setExtents(const glm::vec3& min, const glm::vec3& max);

        [[nodiscard]] const glm::vec3& center() const { return (m_extentsMin + m_extentsMax) * 0.5f; }

        [[nodiscard]] const plot::TickResult& ticks() const { return m_ticks; }

    private:
        BoxStyle m_boxStyle;

        glm::mat4 m_model = glm::mat4(1.0f);
        glm::vec3 m_extentsMin = glm::vec3(-5.0f);
        glm::vec3 m_extentsMax = glm::vec3(5.0f);

        GLuint m_vao = 0;
        GLuint m_vbo = 0;
        GLuint m_ebo = 0;
        Shader m_shader;
        GLint uView = -1;
        GLint uProj = -1;
        GLint uModel = -1;
        GLint uColor = -1;

        plot::TickResult m_ticks{};
        GLuint m_vao_ticks = 0;
        GLuint m_vbo_ticks = 0;

        unsigned int m_ticksX = 11;
        unsigned int m_ticksY = 11;
        unsigned int m_ticksZ = 11;

        plot::EdgeSelection m_currentSelection{};

        void init();

        void update(const Camera& camera);

        void drawWireFrame() const;

        void drawBackFacing() const;

        void drawTickLines() const;

        void drawTicksFront() const;
    };
}
