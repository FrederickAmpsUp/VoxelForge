#include <vforge/gl/base.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>

namespace VoxelForge::GL {

void setup_GL() {
    static bool glInitialized = false;
    if (!glInitialized) {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            throw std::runtime_error("Failed to initialize GLAD. Ensure a GLFW context is active?");
        }

    }
    glInitialized = true;
}
}