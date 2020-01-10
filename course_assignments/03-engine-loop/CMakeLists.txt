cmake_minimum_required(VERSION 3.14)
project(engine-old)

add_library(engine-old STATIC ${CMAKE_CURRENT_LIST_DIR}/engine/src/engine.cpp)
target_include_directories(engine-old PRIVATE ${CMAKE_CURRENT_LIST_DIR}/engine/include/)
target_compile_features(engine-old PUBLIC cxx_std_17)




find_library(sdl2d_lib libSDL2d.a)

if(sdl2d_lib AND CMAKE_BUILD_TYPE STREQUAL Debug)
    message(STATUS "Linking with DEBUG SDL2")
    get_filename_component(debug_path ${sdl2d_lib} DIRECTORY)
    target_include_directories(engine-old PRIVATE ${debug_path}/../include/SDL2)
    target_link_libraries(engine-old 
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

    add_executable(game-old ${CMAKE_CURRENT_LIST_DIR}game/game.cpp)
    target_compile_features(game-old PUBLIC cxx_std_17)
    target_include_directories(game-old PRIVATE ${CMAKE_CURRENT_LIST_DIR}/engine/include/)

    target_link_libraries(game-old PRIVATE engine-old)
else()
    if(NOT sd2_FOUND)
        find_package(sdl2)
        if(NOT sdl2_FOUND)
            find_package(GetSDL2)
        endif()
    endif()
    message(STATUS "Linking with RELEASE SDL2")
    
    target_include_directories(engine-old PRIVATE ${SDL2_INCLUDE_DIRS}
                                                    ${CMAKE_CURRENT_LIST_DIR}/engine/include)
    target_link_libraries(engine-old PRIVATE ${SDL2_LIBRARIES})
    add_executable(game-old ${CMAKE_CURRENT_LIST_DIR}/game/game.cpp)
    target_compile_features(game-old PUBLIC cxx_std_17)
    target_include_directories(game-old PRIVATE ${CMAKE_CURRENT_LIST_DIR}/engine/include/)
    target_link_libraries(game-old PRIVATE engine-old)
endif()


install(TARGETS engine-old game-old)