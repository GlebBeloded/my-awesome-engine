cmake_minimum_required(VERSION 3.14)
project(engine)

add_library(engine STATIC ${CMAKE_CURRENT_LIST_DIR}/engine/src/engine.cpp)
target_include_directories(engine PRIVATE ${CMAKE_CURRENT_LIST_DIR}/engine/include/)
target_compile_features(engine PUBLIC cxx_std_17)




find_library(sdl2d_lib libSDL2d.a)

if(sdl2d_lib AND CMAKE_BUILD_TYPE STREQUAL Debug)
    message(STATUS "Linking with DEBUG SDL2")
    get_filename_component(debug_path ${sdl2d_lib} DIRECTORY)
    target_include_directories(engine PRIVATE ${debug_path}/../include/SDL2)
    target_link_libraries(engine 
    ${sdl2d_lib} 
    -L/usr/local/lib
    -L/usr/lib/x86_64-linux-gnu 
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
    -pthread)

    add_executable(game ${CMAKE_CURRENT_LIST_DIR}/game/game.cpp)
    target_compile_features(game PUBLIC cxx_std_17)
    target_include_directories(game PRIVATE ${CMAKE_CURRENT_LIST_DIR}/engine/include/)

    target_link_libraries(game PRIVATE engine)
else()
    if(NOT sd2_FOUND)
        find_package(sdl2)
        if(NOT sdl2_FOUND)
            find_package(GetSDL2)
        endif()
    endif()
    message(STATUS "Linking with RELEASE SDL2")
    
    target_include_directories(engine PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(engine PRIVATE ${SDL2_LIBRARIES})
    add_executable(game ${CMAKE_CURRENT_LIST_DIR}/game/game.cpp)
    target_compile_features(game PUBLIC cxx_std_17)
    target_include_directories(game PRIVATE ${CMAKE_CURRENT_LIST_DIR}/engine/include/)
    target_link_libraries(game PRIVATE engine)
endif()


install(TARGETS engine game)