add_library(engine SHARED ${CMAKE_CURRENT_LIST_DIR}/engine/src/engine.cpp
                          ${CMAKE_CURRENT_LIST_DIR}/engine/include/engine.hpp
                          ${CMAKE_CURRENT_LIST_DIR}/engine/include/openGL.hpp    
                          ${CMAKE_CURRENT_LIST_DIR}/engine/src/shaders.cpp
                                                                            )


target_include_directories(engine PRIVATE ${CMAKE_CURRENT_LIST_DIR}/engine/include/)

target_compile_features(engine PUBLIC cxx_std_17)

find_package(sdl2)
if(NOT sdl2_FOUND)
    find_package(GetSDL2)
endif()

target_include_directories(engine PRIVATE ${SDL2_INCLUDE_DIRS})

target_link_libraries(engine PRIVATE ${SDL2_LIBRARIES} -lGL)


add_executable(game ${CMAKE_CURRENT_LIST_DIR}/game/main.cpp)
target_compile_features(game PUBLIC cxx_std_17)
target_include_directories(game PRIVATE ${CMAKE_CURRENT_LIST_DIR}/engine/include/)
target_link_libraries(game PRIVATE engine)

install(TARGETS engine game)