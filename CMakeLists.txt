cmake_minimum_required(VERSION 3.10)
project(VoxelForge)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)

# Include directory
include_directories(inc)

# Gather all source files from src/ recursively
file(GLOB_RECURSE SRC_FILES src/*.c src/*.cpp)

# Create a static library from the gathered source files
add_library(deps STATIC ${SRC_FILES})

target_link_libraries(deps glfw3)

# Gather all source files from test/ (non-recursive)
file(GLOB TEST_FILES test/*.c test/*.cpp)

# Create executables for each source file in test/
foreach(TEST_FILE ${TEST_FILES})
    # Get the filename without extension
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    # Create the executable
    add_executable(${TEST_NAME} ${TEST_FILE})
    # Link the executable with the deps library
    target_link_libraries(${TEST_NAME} deps)
endforeach()

# Gather all source files from run/ (non-recursive)
file(GLOB RUN_FILES run/*.c run/*.cpp)

# Create executables for each source file in run/
foreach(RUN_FILE ${RUN_FILES})
    # Get the filename without extension
    get_filename_component(RUN_NAME ${RUN_FILE} NAME_WE)
    # Create the executable
    add_executable(${RUN_NAME} ${RUN_FILE})
    # Link the executable with the deps library
    target_link_libraries(${RUN_NAME} deps)
endforeach()

# Install the run executables
install(TARGETS ${RUN_NAME} DESTINATION /usr/bin)
