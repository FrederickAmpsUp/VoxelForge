#pragma once

#include <glm/glm.hpp>

namespace voxelforge {

struct VoxelData {
    VoxelData(glm::vec3 norm, uint32_t mat) : normal(norm), matID(mat) {}

    glm::vec3 normal;
    uint32_t matID;
};
}