cmake_minimum_required(VERSION 3.20)
project(computer-graphics-experiment)

set(CMAKE_CXX_STANDARD 20)

# 自定头文件
include_directories(include)
# glad glfw glm assimp 头文件
include_directories(include/glad/include)
include_directories(include/glfw/include)
include_directories(include/glm)
include_directories(include/assimp/include)
include_directories(include/soil/include)

# 链接 glfw assimp
link_directories(include/glfw/lib/)
link_directories(include/assimp/lib/)
link_directories(include/soil/lib/)
# 编译 glad
add_subdirectory(include/glad)
add_subdirectory(src)
