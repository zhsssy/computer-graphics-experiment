add_executable(ex2 ex2.cpp)

target_link_libraries(ex2 opengl32)
target_link_libraries(ex2 glad)
target_link_libraries(ex2 debug glfw3d optimized glfw3)