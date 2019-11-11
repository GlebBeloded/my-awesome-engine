cmake_minimum_required(VERSION 3.14.5)
project(hello-lib CXX)

add_library(hello-static STATIC include/hello.hpp src/hello.cpp)

target_include_directories(hello-static PUBLIC
                        ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_features(hello-static PUBLIC cxx_std_17)

add_library(hello-dynamic SHARED include/hello.hpp src/hello.cpp)

target_include_directories(hello-dynamic PUBLIC
                        ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_features(hello-dynamic PUBLIC cxx_std_17)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /std:c++17")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

install(TARGETS hello-static hello-dynamic
        ARCHIVE DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}  #Static library path
        LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}) #Dynamic library path