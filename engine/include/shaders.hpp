#pragma once

#ifdef __ANDROID__
#include <GLES2/gl2.h>
#else
#include "glad.h"
#endif
#include <iosfwd>
#include <string_view>
#include <vector>

void   gl_error_check();
void   check_compile_status(GLuint shader, const char* type);
GLuint comiple_vertex_shader(std::ifstream& shader);
GLuint comiple_fragment_shader(std::ifstream& shader);
GLuint create_shader_program(std::string_view vertex, std::string_view fragment,
                             const std::vector<std::string_view>& attributes);