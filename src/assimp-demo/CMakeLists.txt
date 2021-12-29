add_executable(assimp_demo assimp_demo.cpp)

target_link_libraries(assimp_demo opengl32)
target_link_libraries(assimp_demo glad)
target_link_libraries(assimp_demo debug glfw3d optimized glfw3)
target_link_libraries(assimp_demo assimp)
target_link_libraries(assimp_demo soil)