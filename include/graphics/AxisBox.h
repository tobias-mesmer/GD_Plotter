#pragma once

#include <glad/glad.h>

#include "Shader.h"
#include "graphics/Camera.h"

namespace gdp::graphics {
    class AxisBox {
    public:
        AxisBox();

        ~AxisBox();

        void draw(const Camera& camera) const;

        void setExtents(const glm::vec3& min, const glm::vec3& max);

    private:
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

        std::vector<glm::vec3> m_ticks{};
        GLuint m_vao_ticks = 0;
        GLuint m_vbo_ticks = 0;

        unsigned int m_ticksX = 10;
        unsigned int m_ticksY = 10;
        unsigned int m_ticksZ = 10;

        glm::mat4 m_model = glm::mat4(1.0f);

        void init();

        void drawBox() const;

        void drawTicks() const;
    };
}
