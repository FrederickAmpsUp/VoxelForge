#include <fglw/fglw.hpp>
#include <vforge/vforge.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/noise.hpp>
#include <vox_file/vox_file.h>

class SimpleVoxelRaytraceApp : public fglw::App {
public:
    FGLW_ENABLE_APP;

    virtual void setup(std::vector<const char *> args) override {
        magicavoxel::VoxFile file(false, true, false);
        file.Load("models/Ak74.vox");

        glm::uvec3 largestSize = glm::uvec3(0);

        for (const auto& sparseModel : file.sparseModels()) {
            auto sizeVoxels = sparseModel.size();
            largestSize = glm::max(largestSize, glm::uvec3(sizeVoxels.x / 16 + 1, sizeVoxels.z / 16 + 1, sizeVoxels.y / 16 + 1));
        }

        this->world = std::make_unique<voxelforge::VoxelWorld>(largestSize);
        this->world->clear();

        for (const auto& sparseModel : file.sparseModels()) {
            for (const auto& vox : sparseModel.voxels()) {
                auto voxD = std::make_shared<voxelforge::VoxelData>(glm::vec3(1.0), vox.color);
                this->world->set(glm::uvec3(vox.x, vox.z, vox.y), voxD);
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

        glm::vec3 center = glm::vec3(this->world->size().x / 2, this->world->size().y / 2, this->world->size().z / 2);

        glm::mat4 view = glm::lookAt(center + glm::vec3(2.0f*cos(0.25 * this->win.run_time()), 2.0f, -2.0f*sin(0.25 * this->win.run_time())) * 2.0f, center, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)this->win.width() / (float)this->win.height(), 0.1f, 100.0f);

        this->win.clear(glm::vec3(0.5f, 0.5f, 0.5f));
        this->world->draw(this->win, view, projection);

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
    std::unique_ptr<voxelforge::VoxelWorld> world;
    unsigned long long frameCount = 0;
};

FGLW_DEFINE_APP(
    SimpleVoxelRaytraceApp,
    "Voxel Model Load test",
    1920, 1080
)