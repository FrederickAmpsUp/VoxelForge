#include <fglw/fglw.hpp>
#include <vforge/vforge.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/noise.hpp>

class SimpleVoxelRaytraceApp : public fglw::App {
public:
    FGLW_ENABLE_APP;

    virtual void setup(std::vector<const char *> args) override {
        this->world.clear();

        this->world.setMaterial(0, glm::vec4(0.2f, 1.0f, 0.2f, 1.0f));
        this->world.setMaterial(1, glm::vec4(0.0f, 0.8f, 0.0f, 1.0f));
        for (int x = 0; x < 16 * 64; x++) {
            for (int y = 0; y < 16 * 64; y++) {
                float height = 12.0 * (0.5 + 0.5 * glm::perlin(glm::vec3(x,y, this->win.run_time() * 8.0f) / 16.0f));
                for (int i = 0; i < height; i++) {
                    auto vd = std::make_shared<voxelforge::VoxelData>(glm::vec3(1.0, 0.0, 0.0), height > 6);
                    this->world.set(glm::uvec3(x, i, y), vd);
                }
            }
        }
    }
    
    virtual void update() override {
        static float frameStart = 0;
        
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            FGLW_DEBUG_PRINTF("GL error: %s\n", gluErrorString(err));
        }
        ++frameCount;

        glm::vec3 center = glm::vec3(32, 0, 32);

        glm::mat4 view = glm::lookAt(center + glm::vec3(2.0f*cos(0.25 * this->win.run_time()), 1.5f, -2.0f*sin(0.25 * this->win.run_time())) * 2.0f, center, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)this->win.width() / (float)this->win.height(), 0.1f, 100.0f);

        this->win.clear(glm::vec3(0.5f, 0.5f, 0.5f));
        this->world.draw(this->win, view, projection);

        float frameEnd = this->win.run_time();
        float dt = frameEnd - frameStart;

        if (frameCount % 100 == 0) {
            std::cout << "FPS: " << 1.0 / dt << std::endl;
                    // frames per second * rays per frame = rays per second
            std::cout << "RPS: " << (unsigned long long)(1.0 / dt * this->win.width() * this->win.height()) << std::endl;
        }
        
        frameStart = this->win.run_time();
    }

    virtual void teardown() override {

    }
protected:
    voxelforge::VoxelObject world = voxelforge::VoxelObject(glm::uvec3(64, 1, 64));
    unsigned long long frameCount = 0;
};

FGLW_DEFINE_APP(
    SimpleVoxelRaytraceApp,
    "Voxel Raytracing test",
    1920, 1080
)