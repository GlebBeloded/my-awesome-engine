project(line-render C CXX)



add_library(${PROJECT_NAME} SHARED ${CMAKE_CURRENT_LIST_DIR}/lib/src/ppm.cpp ${CMAKE_CURRENT_LIST_DIR}/lib/src/bresenham.cpp)
target_include_directories(${PROJECT_NAME}  PRIVATE ${CMAKE_CURRENT_LIST_DIR}/lib/include/)
target_compile_features(${PROJECT_NAME}  PUBLIC cxx_std_17)

add_executable(draw-line ${CMAKE_CURRENT_LIST_DIR}/driver_code/main.cpp)

target_compile_features(draw-line PUBLIC cxx_std_17)
target_include_directories(draw-line PRIVATE ${CMAKE_CURRENT_LIST_DIR}/lib/include/)
target_link_libraries(draw-line PRIVATE ${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME} draw-line)