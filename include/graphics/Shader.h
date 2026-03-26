#pragma once
#include <string>
#include "glad/glad.h"

namespace gdp::graphics {
    class Shader {
        public: explicit Shader(std::string_view vs, std::string_view fs);
        ~Shader();
        GLuint m_program{static_cast<GLuint>(-1)};
    };
}
