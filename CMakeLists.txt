cmake_minimum_required(VERSION 3.10)
project(voxelforge
	VERSION 0.0.1
	DESCRIPTION "High-performance voxel rendering library using Vulkan"
	LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 17)

# Metadata exports

# include paths etc for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# export version information with builds
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/version.template"
    "${CMAKE_CURRENT_BINARY_DIR}/.version"
    @ONLY
)

# export version to Git
find_package(Git QUIET)
if (Git_FOUND)
    set(GIT_TAG "v${PROJECT_VERSION}")

    add_custom_target(create_git_tag
        COMMAND ${GIT_EXECUTABLE} tag ${GIT_TAG}
        COMMAND ${GIT_EXECUTABLE} push origin ${GIT_TAG}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Creating and pushing Git tag: ${GIT_TAG}"
        VERBATIM
    )
    
    message(STATUS "Git tag creation target added. Run 'make create_git_tag' to create and push the tag.")
endif()

include_directories(inc)

find_package(Vulkan REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(spdlog REQUIRED)

file(GLOB_RECURSE SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_library(voxelforge STATIC ${SRC_FILES})

target_link_libraries(voxelforge glfw Vulkan::Vulkan spdlog::spdlog)

add_executable(test_app ${CMAKE_SOURCE_DIR}/test/main.cpp)
target_link_libraries(test_app voxelforge)
