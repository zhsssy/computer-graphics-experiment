add_executable(ex3 ex3.cpp)

target_link_libraries(ex3 opengl32)
target_link_libraries(ex3 glad)
target_link_libraries(ex3 debug glfw3d optimized glfw3)
