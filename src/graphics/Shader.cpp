#include "../../include/graphics/Shader.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace gdp::graphics {
    Shader::Shader(const std::string_view vs, const std::string_view fs) {
        const std::filesystem::path p(vs);
        std::cout << "Compiling shader \"" << p.stem().string() << "\"...";

        if (!std::filesystem::exists(vs))
            std::cerr << "Vertex shader path incorrect!" << std::endl;
        if (!std::filesystem::exists(fs))
            std::cerr << "Fragment shader path incorrect!" << std::endl;

        std::string vertSrc = loadFile(vs);
        std::string fragSrc = loadFile(fs);

        auto compile = [](GLenum type, const std::string& src) {
            GLuint shader = glCreateShader(type);
            const char* cstr = src.c_str();
            glShaderSource(shader, 1, &cstr, nullptr);
            glCompileShader(shader);
            return shader;
        };

        GLuint vert = compile(GL_VERTEX_SHADER, vertSrc);
        GLuint frag = compile(GL_FRAGMENT_SHADER, fragSrc);

        m_program = glCreateProgram();
        glAttachShader(m_program, vert);
        glAttachShader(m_program, frag);
        glLinkProgram(m_program);

        glDeleteShader(vert);
        glDeleteShader(frag);

        std::cout << "done!" << std::endl;
    }


    Shader::~Shader() {
        glDeleteProgram(m_program);
    }

     std::string Shader::loadFile(const std::string_view path) {
        std::ifstream file(path.data());
        if (!file.is_open())
            throw std::runtime_error("Failed to open shader file: " + std::string(path));

        return std::string{
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>()
        };
    }
}
