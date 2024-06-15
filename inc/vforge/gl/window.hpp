#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vforge/gl/render.hpp>

namespace VoxelForge::GL {

class Window : public WritableScreen {
public:
    Window(int width, int height, const std::string& title = "VoxelForge Window");

    /**
     * Get the internal handle to the GLFW window used.
     */
    GLFWwindow *handle() const { return _win; }

    /**
     * Get the width of the window.
     */
    int width() const { return _width; }
    /**
     * Set the width of the window.
     */
    void width(int width) { _width = width; }

    /**
     * Get the height of the window.
     */
    int height() const { return _height; }
    /**
     * Set the height of the window.
     */
    void height(int height) { _height = height; }

    /**
     * Get the title of the window.
     */
    std::string title() const { return _title; }
    /**
     * Set the title of the window.
     */
    void title(const std::string& title) { _title = title; }

    /**
     * Run the window's event loop.
     * This is blocking until the window is destroyed.
     */
    void mainloop();
private:
    GLFWwindow *_win;

    int _width, _height;

    std::string _title;
};
}