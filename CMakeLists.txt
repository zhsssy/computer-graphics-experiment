cmake_minimum_required(VERSION 3.20)
project(computer-graphics-experiment)

CMAKE_POLICY(SET CMP0054 NEW) #-- fixes IF() with quoted args

set(CMAKE_CXX_STANDARD 20)

include_directories(glad/include)
include_directories(glfw/include)
include_directories(glm)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/glfw/lib/)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glad)

add_executable(computer-graphics-experiment main.cpp)

target_link_libraries(computer-graphics-experiment opengl32)
target_link_libraries(computer-graphics-experiment glad)
target_link_libraries(computer-graphics-experiment debug glfw3d optimized glfw3)