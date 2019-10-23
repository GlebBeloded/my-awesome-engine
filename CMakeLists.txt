cmake_minimum_required(VERSION 3.10.2)

project(engine CXX)

add_subdirectory(hello-lib)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules
                      ${CMAKE_MODULE_PATH})
set(CLANG_FORMAT_BIN_NAME clang-format)
set(CLANG_FORMAT_EXCLUDE_PATTERNS  "build/" ${CMAKE_BINARY_DIR})
find_package(ClangFormat)

set(CMAKE_CXX_CPPCHECK "cppcheck")