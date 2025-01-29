cmake_minimum_required(VERSION 3.14)
project(voxelforge
	VERSION 0.0.1
	DESCRIPTION "High-performance voxel rendering library using Vulkan"
	LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 17)
include(FetchContent)

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

find_package(spdlog QUIET)
if (NOT spdlog_FOUND)
	message(STATUS "spdlog not found. building from source, this may take some time...")

	FetchContent_Declare(
		spdlog
		GIT_REPOSITORY https://github.com/gabime/spdlog.git
		GIT_TAG	       v1.x
	)

	FetchContent_MakeAvailable(spdlog)
else()
	message(STATUS "found spdlog development libraries")
endif()

file(GLOB_RECURSE SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_library(voxelforge STATIC ${SRC_FILES})

target_link_libraries(voxelforge Vulkan::Vulkan spdlog::spdlog)

option(BUILD_TESTS "Build test programs alongside voxelforge" ON)
if (BUILD_TESTS)
	message(STATUS "Building tests...")
	add_subdirectory(tests)
endif()
