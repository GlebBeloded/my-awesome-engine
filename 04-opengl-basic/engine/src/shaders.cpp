
#include <algorithm>
#include <array>
#include <cassert>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "shaders.hpp"

GLuint comiple_vertex_shader(std::ifstream& shader) {
    GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
    gl_error_check();

    std::string file{ std::istreambuf_iterator<char>(shader),
                      std::istreambuf_iterator<char>() };

    const char* source = file.data();
    glShaderSource(vert_shader, 1, &source, nullptr);
    gl_error_check();

    glCompileShader(vert_shader);
    gl_error_check();

    check_compile_status(vert_shader, "vertex");

    return vert_shader;
}

GLuint comiple_fragment_shader(std::ifstream& shader) {

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    gl_error_check();

    std::string file{ std::istreambuf_iterator<char>(shader),
                      std::istreambuf_iterator<char>() };
    const char* source = file.data();

    glShaderSource(fragment_shader, 1, &source, nullptr);
    gl_error_check();
    char* check = new char[500];

    glCompileShader(fragment_shader);
    gl_error_check();

    check_compile_status(fragment_shader, "fragment");

    return fragment_shader;
}

void check_compile_status(GLuint shader, const char* type) {
    GLint compiled_status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled_status);
    gl_error_check();

    if (compiled_status == 0) {
        GLint info_len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);
        gl_error_check();
        std::vector<char> info_chars(static_cast<size_t>(info_len));
        glGetShaderInfoLog(shader, info_len, nullptr, info_chars.data());
        gl_error_check();
        glDeleteShader(shader);
        gl_error_check();

        std::string error_message{ "Error compiling " };
        error_message += type;
        error_message += " shader \n";

        throw std::runtime_error(error_message);
    }
}

GLuint create_shader_program(std::filesystem::path                vertex,
                             std::filesystem::path                fragment,
                             const std::vector<std::string_view>& attributes) {
    std::ifstream file{};
    file.exceptions(std::ios_base::failbit);

    file.open(vertex.c_str());
    auto vert_shader = comiple_vertex_shader(file);
    file.close();

    file.open(fragment.c_str());
    auto fragment_shader = comiple_fragment_shader(file);
    file.close();

    auto program_id_ = glCreateProgram();
    gl_error_check();
    if (0 == program_id_) {
        throw std::runtime_error("failed to create gl program");
    }

    glAttachShader(program_id_, vert_shader);
    gl_error_check();
    glAttachShader(program_id_, fragment_shader);
    gl_error_check();

    // bind attribute location
    for (auto i = 0; i < attributes.size(); i++)
        glBindAttribLocation(program_id_, i, attributes[i].data());
    gl_error_check();
    // link program after binding attribute locations
    glLinkProgram(program_id_);

    gl_error_check();
    // Check the link status
    GLint linked_status = 0;
    glGetProgramiv(program_id_, GL_LINK_STATUS, &linked_status);
    gl_error_check();

    if (linked_status == 0) {
        GLint infoLen = 0;
        glGetProgramiv(program_id_, GL_INFO_LOG_LENGTH, &infoLen);
        gl_error_check();
        std::vector<char> infoLog(static_cast<size_t>(infoLen));
        glGetProgramInfoLog(program_id_, infoLen, nullptr, infoLog.data());
        gl_error_check();
        std::cerr << "Error linking program:\n" << infoLog.data();
        glDeleteProgram(program_id_);

        const int err = static_cast<int>(glGetError());
        if (err != GL_NO_ERROR)
            throw std::logic_error("OpenGL failed to load");
    }
    return program_id_;
}