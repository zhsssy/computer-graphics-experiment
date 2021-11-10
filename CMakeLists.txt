cmake_minimum_required(VERSION 3.20)
project(computer-graphics-experiment)

#CMAKE_POLICY(SET CMP0054 NEW) #-- fixes IF() with quoted args
#CMAKE_POLICY(SET CMP0057 NEW) #-- enables IF(.. IN_LIST ..)

set(CMAKE_CXX_STANDARD 17)

execute_process(
        COMMAND ${CMAKE_COMMAND} -E environment
)

# 检查环境变量
if (NOT DEFINED ENV{GLFW_HOME})
    message(FATAL_ERROR "found no env named GLFW_HOME")
endif()
if (NOT DEFINED ENV{GLAD_HOME})
    message(FATAL_ERROR "found no env named GLAD_HOME")
endif()
if (NOT DEFINED ENV{GLM_HOME})
    message(FATAL_ERROR "found no env named GLM_HOME")
endif()

SET(GLFW_HOME "$ENV{GLFW_HOME}")
SET(GLAD_HOME "$ENV{GLAD_HOME}")
SET(GLM_HOME "$ENV{GLM_HOME}/glm")

# 设置头文件目录
include_directories("${GLFW_HOME}/include")
include_directories("${GLAD_HOME}/include")
include_directories(${GLM_HOME})

# 添加 GLFW3 预编译库
add_library(glfw SHARED IMPORTED)
SET_TARGET_PROPERTIES(glfw PROPERTIES IMPORTED_LOCATION "${GLFW_HOME}/lib/libglfw.3.dylib")


# 创建可执行文件
add_executable(computer-graphics-experiment main.cpp "lib/glad.c")

# 链接 GLFW GLAD OpenGL
target_link_libraries(computer-graphics-experiment glfw "-framework OpenGL")
