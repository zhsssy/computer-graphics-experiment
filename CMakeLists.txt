cmake_minimum_required(VERSION 3.20)
project(computer-graphics-experiment)

set(CMAKE_CXX_STANDARD 20)

# 自定头文件
include_directories(include)
# glad glfw glm assimp 头文件
include_directories(glad/include)
include_directories(glfw/include)
include_directories(glm)
include_directories(assimp/include)

# 链接 glfw assimp
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/glfw/lib/)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/assimp/lib/)
# 编译 glad
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glad)

add_subdirectory(src)

add_executable(computer-graphics-experiment main.cpp)

target_link_libraries(computer-graphics-experiment opengl32)
target_link_libraries(computer-graphics-experiment glad)
target_link_libraries(computer-graphics-experiment debug glfw3d optimized glfw3)
target_link_libraries(computer-graphics-experiment assimp)