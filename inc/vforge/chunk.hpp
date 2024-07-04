#pragma once

#include <glm/glm.hpp>
#include <fglw/fglw.hpp>
#include <vforge/voxel.hpp>
#include <memory>

namespace voxelforge {

class VoxelSubChunk {
public:
    VoxelSubChunk();

    void set(unsigned int x, unsigned int y, unsigned int z, std::shared_ptr<VoxelData> data);
    void clear(unsigned int x, unsigned int y, unsigned int z);

    void set(glm::uvec3 position, std::shared_ptr<VoxelData> data) { this->set(position.x, position.y, position.z, data); }
    void clear(glm::uvec3 position) { this->clear(position.x, position.y, position.z); }

    void clear();

    uint64_t getBitmask() const { return this->bitmask; }
private:
    uint64_t bitmask;
    std::shared_ptr<VoxelData> data[4][4][4];
};

class VoxelChunk {
public:
    VoxelChunk();

    void set(unsigned int x, unsigned int y, unsigned int z, std::shared_ptr<VoxelData> data);
    void clear(unsigned int x, unsigned int y, unsigned int z);

    void set(glm::uvec3 position, std::shared_ptr<VoxelData> data) { this->set(position.x, position.y, position.z, data); }
    void clear(glm::uvec3 position) { this->clear(position.x, position.y, position.z); }

    void clear();

    std::shared_ptr<VoxelSubChunk> getSubChunk(unsigned int x, unsigned int y, unsigned int z);
    std::shared_ptr<VoxelSubChunk> getSubChunk(glm::uvec3 pos) { return this->getSubChunk(pos.x, pos.y, pos.z); }

    uint64_t getBitmask() const { return this->bitmask; }
private:
    uint64_t bitmask;
    std::shared_ptr<VoxelSubChunk> data[4][4][4];
};
}