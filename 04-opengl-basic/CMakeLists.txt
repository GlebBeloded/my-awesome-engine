cmake_minimum_required(VERSION 3.14)

add_library(matrix SHARED ${CMAKE_CURRENT_LIST_DIR}/matrix/src/matrix.cpp)

target_include_directories(matrix PUBLIC ${CMAKE_CURRENT_LIST_DIR}/matrix/include/)
target_compile_features(matrix PUBLIC cxx_std_17)


add_library(openGL SHARED ${CMAKE_CURRENT_LIST_DIR}/openGL/src/glad.c)

target_include_directories(openGL PUBLIC ${CMAKE_CURRENT_LIST_DIR}/openGL/include/)
target_compile_features(openGL PUBLIC cxx_std_17)

add_library(engine SHARED ${CMAKE_CURRENT_LIST_DIR}/engine/src/engine.cpp
                          ${CMAKE_CURRENT_LIST_DIR}/engine/src/shaders.cpp)
                          
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_LIST_DIR}/engine/include/ 
                                          ${CMAKE_CURRENT_LIST_DIR}/openGL/include/ 
                                          ${CMAKE_CURRENT_LIST_DIR}/matrix/include/)



target_compile_features(engine PUBLIC cxx_std_17)

find_package(sdl2)
if(NOT sdl2_FOUND)
    find_package(GetSDL2)
endif()

target_include_directories(engine PUBLIC ${SDL2_INCLUDE_DIRS})


target_link_libraries(engine PUBLIC matrix)
target_link_libraries(engine PUBLIC ${SDL2_LIBRARIES})
target_link_libraries(engine PUBLIC openGL -lGL)

add_library(tetris SHARED ${CMAKE_CURRENT_LIST_DIR}/game/src/types.cpp 
                          ${CMAKE_CURRENT_LIST_DIR}/game/src/pieces.cpp 
                          ${CMAKE_CURRENT_LIST_DIR}/game/src/state.cpp
                          ${CMAKE_CURRENT_LIST_DIR}/game/include/types.hpp 
                          ${CMAKE_CURRENT_LIST_DIR}/game/include/pieces.hpp 
                          ${CMAKE_CURRENT_LIST_DIR}/game/include/state.hpp)

target_include_directories(tetris PUBLIC ${CMAKE_CURRENT_LIST_DIR}/engine/include/ 
                                          ${CMAKE_CURRENT_LIST_DIR}/matrix/include/
                                          ${CMAKE_CURRENT_LIST_DIR}/game/include/)





add_executable(game ${CMAKE_CURRENT_LIST_DIR}/game/main.cpp)
target_compile_features(game PUBLIC cxx_std_17)
target_include_directories(game PUBLIC ${CMAKE_CURRENT_LIST_DIR}/engine/include/)
target_link_libraries(game PUBLIC engine)

install(TARGETS matrix openGL engine game)

file(COPY ${CMAKE_CURRENT_LIST_DIR}/data/shaders/ DESTINATION ${CMAKE_BINARY_DIR}/shaders)
file(COPY ${CMAKE_CURRENT_LIST_DIR}/data/textures/ DESTINATION ${CMAKE_BINARY_DIR}/textures)