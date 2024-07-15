#include <vforge/world.hpp>

namespace voxelforge {

void VoxelWorld::draw(fglw::RenderTarget& fb, glm::mat4x4 view, glm::mat4x4 proj) {
    for (const auto& object : this->objects) {
        if (!object) continue;
        object->draw(fb, view, proj);
    }
}
}