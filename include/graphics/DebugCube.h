#pragma once

#include <glad/glad.h>

#include "Shader.h"
#include "graphics/Camera.h"

namespace gdp::graphics {
    class DebugCube {
    public:
        DebugCube();
        ~DebugCube();

        void drawDebugCube(const Camera& camera) const;

    private:
        GLuint m_vao = 0;
        GLuint m_vbo = 0;
        GLuint m_ebo = 0;
        Shader m_shader;

        void initCube();
    };

}
