add_executable(ex1 ex1.cpp)

target_link_libraries(ex1 opengl32)
target_link_libraries(ex1 glad)
target_link_libraries(ex1 debug glfw3d optimized glfw3)