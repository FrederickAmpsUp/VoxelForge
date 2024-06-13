#include <vforge/gl/window.hpp>

int main(int argc, char **argv) {
    VoxelForge::GL::Window window(800, 600, "Test Window");

    window.mainloop();

    return 0;
}