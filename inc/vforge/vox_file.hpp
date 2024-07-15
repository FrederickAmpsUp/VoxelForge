#pragma once

#include <vforge/object.hpp>
#include <vforge/world.hpp>
#include <memory>

namespace voxelforge::files {

/**
 * UNFINISHED, DO NOT USE
 */
class MagicaVoxelVOX {
public:
    MagicaVoxelVOX(const char *filename);

    std::shared_ptr<voxelforge::VoxelWorld> getWorld() { return this->world; }
private:
    std::shared_ptr<voxelforge::VoxelWorld> world;
};
}