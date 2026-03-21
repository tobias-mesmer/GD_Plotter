#pragma once

#include <glad/glad.h>
#include "graphics/Camera.h"

namespace gdp::graphics {
    class GridRenderer {
    public:
        GridRenderer();
        ~GridRenderer();

        void drawDebugCube(const Camera& camera) const;

    private:
        GLuint m_vao = 0;
        GLuint m_vbo = 0;
        GLuint m_ebo = 0;
        GLuint m_shader = 0;

        void initCube();
        GLuint createShaderProgram(const char* vs, const char* fs);
    };

}
