project(triangle-render C CXX)



add_library(${PROJECT_NAME} SHARED 
                            ${CMAKE_CURRENT_LIST_DIR}/../04-01-lines/lib/src/ppm.cpp 
                            #${CMAKE_CURRENT_LIST_DIR}/../04-01-lines/lib/src/bresenham.cpp 
                            ${CMAKE_CURRENT_LIST_DIR}/lib/src/renderer.cpp
                            )
target_include_directories(${PROJECT_NAME}  PRIVATE 
                            ${CMAKE_CURRENT_LIST_DIR}/lib/include/
                            ${CMAKE_CURRENT_LIST_DIR}/../04-01-lines/lib/include
                            )
target_compile_features(${PROJECT_NAME}  PUBLIC cxx_std_17)
target_link_libraries(${PROJECT_NAME} line-render)

add_executable(draw-triangle ${CMAKE_CURRENT_LIST_DIR}/driver_code/main.cpp)

target_compile_features(draw-triangle PUBLIC cxx_std_17)
target_include_directories(draw-triangle PRIVATE 
                            ${CMAKE_CURRENT_LIST_DIR}/lib/include/
                            ${CMAKE_CURRENT_LIST_DIR}/../04-01-lines/lib/include)

target_link_libraries(draw-triangle PRIVATE ${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME} draw-triangle)