#include <fglw/fglw.hpp>
#include <iostream>

class FGLWTestApp : public fglw::App {
public:
    FGLW_ENABLE_APP;

    void setup(std::vector<const char *> args) {
    }

    void update() {
        float t = this->win.run_time();
        glm::vec3 color(cos(t), sin(t), 1.0);
        this->win.clear(color);
    }

    void teardown() {
    }
};

FGLW_DEFINE_APP(
    FGLWTestApp,
    "VoxelForge FGLW Test App",
    800, 600
)