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
        //file.Load("models/tiger1.vox");
/*
        glm::uvec3 largestSize = glm::uvec3(0);

        for (const auto& sparseModel : file.sparseModels()) {
            auto sizeVoxels = sparseModel.size();
            largestSize = glm::max(largestSize, glm::uvec3(sizeVoxels.x / 16 + 1, sizeVoxels.z / 16 + 1, sizeVoxels.y / 16 + 1));
        }

        FGLW_DEBUG_PRINTF("size: %u %u %u\n", largestSize.x, largestSize.y, largestSize.z);

        this->world = std::make_unique<voxelforge::VoxelObject>(largestSize);
        this->world->clear();
        
        uint64_t voxCount = 0;

        for (const auto& sparseModel : file.sparseModels()) {
            for (const auto& vox : sparseModel.voxels()) {
                auto voxD = std::make_shared<voxelforge::VoxelData>(glm::vec3(1.0), vox.color);
                this->world->set(glm::uvec3(vox.x, vox.z, vox.y), voxD);
                voxCount++;
            }

            magicavoxel::Palette p = sparseModel.palette();
            for (int i = 0; i < 256; i++) {
                auto color = p[i];
                glm::vec4 cD = glm::vec4(color.r, color.g, color.b, color.a) / 256.0f;
                this->world->setMaterial(i, cD);
            }
        }

        std::cout << voxCount << " voxels" << std::endl;*/
        voxelforge::files::MagicaVoxelVOX file("models/tiger1.vox");
        this->world = file.getWorld();
    }
    
    virtual void update() override {
        static float frameStart = 0;

        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            FGLW_DEBUG_PRINTF("GL error: %s\n", gluErrorString(err));
        }
        ++frameCount;

        glm::vec3 center = glm::vec3(0, 4, 0);
        float r = 3.0f;
        glm::mat4 view = glm::lookAt(center + glm::vec3(r*cos(0.25 * this->win.run_time()), r, r*sin(0.25 * this->win.run_time())) * 2.0f, center, glm::vec3(0.0f, 1.0f, 0.0f));
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
    std::shared_ptr<voxelforge::VoxelWorld> world;
    //magicavoxel::VoxFile file = magicavoxel::VoxFile(false, true, true);
    unsigned long long frameCount = 0;
};

FGLW_DEFINE_APP(
    SimpleVoxelRaytraceApp,
    "Voxel Model Load test",
    1920, 1080
)