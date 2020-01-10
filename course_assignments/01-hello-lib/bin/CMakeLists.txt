cmake_minimum_required(VERSION 3.14.5)
project(hello-bin CXX)

add_executable(hello-bin-static src/main.cpp)
target_compile_features(hello-bin-static PRIVATE cxx_std_17)
target_link_libraries(hello-bin-static LINK_PUBLIC hello-static "-static")

add_executable(hello-bin-dynamic src/main.cpp)
target_compile_features(hello-bin-dynamic PRIVATE cxx_std_17)
target_link_libraries(hello-bin-dynamic LINK_PUBLIC hello-dynamic)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /std:c++17")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

install(TARGETS hello-bin-static hello-bin-dynamic
        RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})
