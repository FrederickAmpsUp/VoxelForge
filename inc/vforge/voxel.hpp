#pragma once

#include <glm/glm.hpp>

namespace voxelforge {

    // can add more stuff here if needed
struct VoxelData {
    VoxelData(glm::vec3 col) : color(col) {}

    glm::vec3 color;
};
}