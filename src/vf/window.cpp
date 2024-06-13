#include <vforge/gl/window.hpp>
#include <vforge/gl/base.hpp>

#include <stdexcept>

namespace VoxelForge::GL {

static void vfgl_window_frmbf_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

Window::Window(int width, int height, const std::string& title) : _width(width), _height(height), _title(title) {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }
    
    this->_win = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (this->_win == nullptr) {
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(this->_win);

    VoxelForge::GL::setup_GL(); // make sure the graphics library is initialized

    glfwSetFramebufferSizeCallback(this->_win, vfgl_window_frmbf_size_callback);

    glViewport(0, 0, width, height);
}

void Window::mainloop() {
        // TODO: keyboard stuff
    while (!glfwWindowShouldClose(this->_win)) {
        glfwMakeContextCurrent(this->_win);

        glClearColor(1,0,1,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwSwapBuffers(this->_win);

        glfwPollEvents();
    }
}
}