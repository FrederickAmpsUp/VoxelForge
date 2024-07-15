#pragma once

#include <vforge/object.hpp>
#include <vforge/worldobject.hpp>
#include <vector>

namespace voxelforge {

class VoxelWorld : public voxelforge::WorldObject {
public:
    VoxelWorld(std::vector<std::shared_ptr<voxelforge::VoxelObject>> objects) : objects(objects) {}
    VoxelWorld() : objects(0) {}

    uint32_t addObject(std::shared_ptr<voxelforge::VoxelObject> obj) {
        objects.push_back(obj);
        return objects.size() - 1;
    }

    virtual void draw(fglw::RenderTarget& fb, glm::mat4x4 view, glm::mat4x4 proj) override;
private:
    std::vector<std::shared_ptr<voxelforge::VoxelObject>> objects;
};
}