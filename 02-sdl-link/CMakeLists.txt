cmake_minimum_required(VERSION 3.15.4)

project(sdl-link C CXX)

add_executable(sdl-link-static src/main.cpp)
target_compile_features(sdl-link-static PUBLIC cxx_std_17)


find_package(sdl2)

if(NOT sdl2_FOUND)
    find_package(GetSDL2)
endif()

target_include_directories(sdl-link-static PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(sdl-link-static PRIVATE ${SDL2_LIBRARIES})

install(TARGETS sdl-link-static)

#find_package(sdl2d) won't work