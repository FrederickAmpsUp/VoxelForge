#include <fglw/fglw.hpp>
#include <vforge/vforge.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/noise.hpp>

class SimpleVoxelRaytraceApp : public fglw::App {
public:
    FGLW_ENABLE_APP;

    virtual void setup(std::vector<const char *> args) override {
        auto vd = std::make_shared<voxelforge::VoxelData>(glm::vec3(1.0, 0.0, 0.0));
        this->world.clear();
        for (int x = 0; x < 64 * 16; x++) {
            for (int y = 0; y < 64 * 16; y++) {
                float height = 12.0 * (0.5 + 0.5 * glm::perlin(glm::vec3(x,y, this->win.run_time() * 8.0f) / 16.0f));
                for (int i = 0; i < height; i++) {
                    this->world.set(glm::uvec3(x, i, y), vd);
                }
            }
        }
    }

    virtual void update() override {
        float frameStart = this->win.run_time();
        this->win.clear(glm::vec3(0.5f, 0.5f, 0.5f));

        glm::vec3 center = glm::vec3(32, 0, 32);

        glm::mat4 view = glm::lookAt(center + glm::vec3(2.0f*cos(0.25 * this->win.run_time()), 0.6f, -2.0f*sin(0.25 * this->win.run_time())) * 2.0f, center, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)this->win.width() / (float)this->win.height(), 0.1f, 100.0f);

        this->world.draw(this->win, view, projection);

        float frameEnd = this->win.run_time();
        float dt = frameEnd - frameStart;

        if (frameCount % 100 == 0) {
            std::cout << "FPS: " << 1.0 / dt << std::endl;
                    // frames per second * rays per frame = rays per second
            std::cout << "RPS: " << (unsigned long long)(1.0 / dt * this->win.width() * this->win.height()) << std::endl;
        }
        
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            FGLW_DEBUG_PRINTF("GL error: %s\n", gluErrorString(err));
        }
        ++frameCount;
    }

    virtual void teardown() override {

    }
protected:
    voxelforge::VoxelWorld world = voxelforge::VoxelWorld(glm::uvec3(64, 1, 64));
    unsigned long long frameCount = 0;
};

FGLW_DEFINE_APP(
    SimpleVoxelRaytraceApp,
    "Voxel Raytracing test",
    1920, 1080
)