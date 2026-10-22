#pragma once
#include <string>
#include <filesystem>
#include "glad/glad.h"

namespace gdp::graphics {
    class Shader {
        public: explicit Shader(std::string_view vs, std::string_view fs);
        ~Shader();
        static std::string loadFile(const std::filesystem::path& path);
        GLuint m_program{0};
    };
}
