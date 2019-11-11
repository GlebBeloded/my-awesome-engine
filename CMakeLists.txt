cmake_minimum_required(VERSION 3.14.5)

project(engine CXX)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules
                      ${CMAKE_MODULE_PATH})
set(CLANG_FORMAT_BIN_NAME clang-format)
set(CLANG_FORMAT_EXCLUDE_PATTERNS  "build/" ${CMAKE_BINARY_DIR})
get_filename_component(ROOT_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)

set(CMAKE_CXX_CPPCHECK "cppcheck")

#formats source files
find_package(ClangFormat)
#gets us subdir_list containig full path to subprojects
find_package(GetSubdirs)

#adds subprojects to be processed by cmake
foreach(target_dir ${subdir_list})
    get_filename_component(target_name ${target_dir} NAME)
    add_subdirectory(${target_name})
endforeach()
