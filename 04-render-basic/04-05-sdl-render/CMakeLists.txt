project(sdl-render C CXX)




add_executable(sdl-render  ${CMAKE_CURRENT_LIST_DIR}/main.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/../04-04-interpolated/lib/src/triangle.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/../04-01-lines/lib/src/ppm.cpp)



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

else()
    if(NOT sd2_FOUND)
        find_package(sdl2)
        if(NOT sdl2_FOUND)
            find_package(GetSDL2)
        endif()
    endif()
    message(STATUS "Linking with RELEASE SDL2")
endif()
target_compile_features(sdl-render PUBLIC cxx_std_17)
target_include_directories(sdl-render PRIVATE 
                            ${CMAKE_CURRENT_LIST_DIR}/../04-01-lines/lib/include/  
                            ${CMAKE_CURRENT_LIST_DIR}/../04-04-interpolated/lib/include
                            ${SDL2_INCLUDE_DIRS})

target_link_libraries(sdl-render PRIVATE
                        ${SDL2_LIBRARIES})
    
install(TARGETS sdl-render)