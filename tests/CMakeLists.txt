find_package(glfw3 3.3 REQUIRED)

add_executable(test_setup test_setup.cpp)
target_link_libraries(test_setup voxelforge glfw)
