add_executable(ex4 ex4.cpp)

target_link_libraries(ex4 opengl32)
target_link_libraries(ex4 glad)
target_link_libraries(ex4 debug glfw3d optimized glfw3)