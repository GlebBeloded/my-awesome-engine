project(triangle-vertex-interpolated C CXX)



add_library(${PROJECT_NAME} SHARED 
                            ${CMAKE_CURRENT_LIST_DIR}/lib/src/triangle.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/../04-01-lines/lib/src/ppm.cpp  
                            )
target_include_directories(${PROJECT_NAME}  PRIVATE 
                            ${CMAKE_CURRENT_LIST_DIR}/lib/include/
                            ${CMAKE_CURRENT_LIST_DIR}/../04-01-lines/lib/include
                            ${CMAKE_CURRENT_LIST_DIR}../04-01-lines/lib/include/
                            )
target_compile_features(${PROJECT_NAME}  PUBLIC cxx_std_17)

add_executable(draw-interpolated  ${CMAKE_CURRENT_LIST_DIR}/driver_code/main.cpp)

target_compile_features(draw-interpolated PUBLIC cxx_std_17)
target_include_directories(draw-interpolated PRIVATE 
                            ${CMAKE_CURRENT_LIST_DIR}/lib/include/
                            ${CMAKE_CURRENT_LIST_DIR}/../04-01-lines/lib/include)

target_link_libraries(draw-interpolated PRIVATE ${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME} draw-interpolated)