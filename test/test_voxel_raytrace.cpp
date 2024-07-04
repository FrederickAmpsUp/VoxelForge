#include <fglw/fglw.hpp>
#include <vforge/vforge.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class SimpleVoxelRaytraceApp : public fglw::App {
public:
    FGLW_ENABLE_APP;

    virtual void setup(std::vector<const char *> args) override {
    }

    virtual void update() override {
        this->world.set(glm::uvec3(0, 8, 0),
            std::make_shared<voxelforge::VoxelData>(glm::vec3(1.0, 0.0, 0.0))
        );
        this->win.clear(glm::vec3(0.0f, 0.0f, 0.0f));

        glm::mat4 view = glm::lookAt(glm::vec3(2.0f*cos(this->win.run_time()), 2.0f, -2.0f*sin(this->win.run_time())) * 2.0f, glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)this->win.width() / (float)this->win.height(), 0.1f, 100.0f);

        this->world.draw(this->win, view, projection);
        
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            FGLW_DEBUG_PRINTF("GL error: %s\n", gluErrorString(err));
        }
    }

    virtual void teardown() override {

    }
protected:
    voxelforge::VoxelWorld world = voxelforge::VoxelWorld(glm::uvec3(1, 1, 1));
};

FGLW_DEFINE_APP(
    SimpleVoxelRaytraceApp,
    "Voxel Raytracing test",
    800, 600
)