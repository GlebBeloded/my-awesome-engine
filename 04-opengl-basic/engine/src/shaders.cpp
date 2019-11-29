#include "shaders.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

GLuint comiple_vertex_shader(std::string_view shader) {
    GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
    gl_error_check();

    const char* source = shader.data();
    glShaderSource(vert_shader, 1, &source, nullptr);
    gl_error_check();

    glCompileShader(vert_shader);
    gl_error_check();

    check_compile_status(vert_shader, "vertex");

    return vert_shader;
}

GLuint comiple_fragment_shader(std::string_view shader) {
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    gl_error_check();

    const char* source = shader.data();
    glShaderSource(fragment_shader, 1, &source, nullptr);
    gl_error_check();

    glCompileShader(fragment_shader);
    gl_error_check();

    // check_compile_status(fragment_shader, "fragment");

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

        // error_message += info_chars.data();

        throw std::runtime_error(error_message);
    }
}
