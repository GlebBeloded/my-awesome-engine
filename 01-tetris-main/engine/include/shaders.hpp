#pragma once

#include "glad.h"
#include <filesystem>
#include <iosfwd>
#include <string_view>
#include <vector>

void   gl_error_check();
void   check_compile_status(GLuint shader, const char* type);
GLuint comiple_vertex_shader(std::ifstream& shader);
GLuint comiple_fragment_shader(std::ifstream& shader);
GLuint create_shader_program(std::filesystem::path                vertex,
                             std::filesystem::path                fragment,
                             const std::vector<std::string_view>& attributes);