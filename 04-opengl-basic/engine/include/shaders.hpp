#pragma once

#include "openGL.hpp"
#include <iosfwd>
#include <string>
#include <string_view>

void   gl_error_check();
void   check_compile_status(GLuint shader, const char* type);
GLuint comiple_vertex_shader(std::string_view shader);
GLuint comiple_fragment_shader(std::string_view shader);