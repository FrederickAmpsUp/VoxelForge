#pragma once

#include <glm/glm.hpp>
#include <vforge/worldobject.hpp>
#include <vforge/chunk.hpp>
#include <unordered_map>
#include <fglw/fglw.hpp>

namespace voxelforge {

namespace internal {
struct uvec3Hash {
    size_t operator()(const glm::uvec3& v) const {
        size_t hash = 0;
        
        // Combine hashes of individual components
        hash ^= std::hash<unsigned int>{}(v.x) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<unsigned int>{}(v.y) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<unsigned int>{}(v.z) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        
        return hash;
    }
};
}
}