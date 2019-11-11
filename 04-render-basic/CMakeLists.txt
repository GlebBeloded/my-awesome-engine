project(render)

add_library(PPM STATIC ${CMAKE_CURRENT_LIST_DIR}/lib/src/ppm.cpp)
target_include_directories(PPM PRIVATE ${CMAKE_CURRENT_LIST_DIR}/lib/include/)
target_compile_features(PPM PUBLIC cxx_std_17)

add_library(BRESENHAM STATIC ${CMAKE_CURRENT_LIST_DIR}/lib/src/bresenham.cpp)
target_include_directories(BRESENHAM PRIVATE ${CMAKE_CURRENT_LIST_DIR}/lib/include/)
target_compile_features(BRESENHAM PUBLIC cxx_std_17)

add_executable(test ${CMAKE_CURRENT_LIST_DIR}/driver_code/main.cpp)

target_compile_features(test PUBLIC cxx_std_17)
target_include_directories(test PRIVATE ${CMAKE_CURRENT_LIST_DIR}/lib/include/)
target_link_libraries(test PRIVATE PPM BRESENHAM)

install(TARGETS PPM test)