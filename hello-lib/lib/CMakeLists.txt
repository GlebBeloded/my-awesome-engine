cmake_minimum_required(VERSION 3.10.2)
project(hello-lib CXX)

add_library(hello-lib-static STATIC include/hello.hpp src/hello.cpp)

target_include_directories(hello-lib-static PUBLIC
                        ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_features(hello-lib-static PUBLIC cxx_std_17)

add_library(hello-lib-dynamic SHARED include/hello.hpp src/hello.cpp)

target_include_directories(hello-lib-dynamic PUBLIC
                        ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_features(hello-lib-dynamic PUBLIC cxx_std_17)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /std:c++17")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

install(TARGETS hello-lib-static hello-lib-dynamic
        ARCHIVE DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}  #Static library path
        LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}) #Dynamic library path