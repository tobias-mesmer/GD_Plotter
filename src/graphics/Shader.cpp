#include "../../include/graphics/Shader.h"

namespace gdp::graphics {
    Shader::Shader(const std::string_view vs, const std::string_view fs) {
        auto compile = [](GLenum type, const char* src) {
            GLuint shader = glCreateShader(type);
            glShaderSource(shader, 1, &src, nullptr);
            glCompileShader(shader);
            return shader;
        };

        GLuint vert = compile(GL_VERTEX_SHADER, vs.data());
        GLuint frag = compile(GL_FRAGMENT_SHADER, fs.data());

        m_program = glCreateProgram();
        glAttachShader(m_program, vert);
        glAttachShader(m_program, frag);
        glLinkProgram(m_program);

        glDeleteShader(vert);
        glDeleteShader(frag);
    }

    Shader::~Shader() {
        glDeleteProgram(m_program);
    }
}
