cmake_minimum_required(VERSION 3.15.4)

project(sdl-link C CXX)

add_executable(sdl-link-dynamic src/main.cpp)
list(APPEND target_list sdl-link-dynamic)
target_compile_features(sdl-link-dynamic PUBLIC cxx_std_17)


find_package(sdl2)

if(NOT sdl2_FOUND)
    find_package(GetSDL2)
endif()

target_include_directories(sdl-link-dynamic PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(sdl-link-dynamic PRIVATE ${SDL2_LIBRARIES})

find_library(sdl2d_lib libSDL2d.a)

if(sdl2d_lib)

    get_filename_component(debug_path ${sdl2d_lib} DIRECTORY)
    add_executable(sdl-link-static-debug src/main.cpp)
    list(APPEND target_list sdl-link-static-debug)
    target_compile_features(sdl-link-static-debug PUBLIC cxx_std_17)
    target_include_directories(sdl-link-static-debug PRIVATE ${debug_path}/../include/SDL2)
    target_link_libraries(sdl-link-static-debug 
    ${sdl2d_lib}
    -L/usr/local/lib
    -Ldir/usr/lib/x86_64-linux-gnu 
    -Wl,-rpath,/usr/local/lib 
    -Wl,--enable-new-dtags 
    -lglib-2.0 
    -lgobject-2.0 
    -lgio-2.0 
    -libus-1.0 
    -ldbus-1 
    /usr/lib/x86_64-linux-gnu/libsndio.so
    -ldl 
    -lm 
    -Wl,--no-undefined 
    -pthread
)
endif()

install(TARGETS ${target_list})