add_executable(ex5 ex5.cpp)

target_link_libraries(ex5 opengl32)
target_link_libraries(ex5 glad)
target_link_libraries(ex5 debug glfw3d optimized glfw3)