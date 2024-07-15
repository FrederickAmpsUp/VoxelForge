#include <vforge/chunk.hpp>

namespace voxelforge {

VoxelSubChunk::VoxelSubChunk() {
    this->bitmask = 0;
}

VoxelChunk::VoxelChunk() {
    this->bitmask = 0;
}

void VoxelSubChunk::set(unsigned int x, unsigned int y, unsigned int z, std::shared_ptr<VoxelData> vox) {
    if (x > 3 || y > 3 || z > 3) return; // voxel out of bounds
    this->data[x][y][z] = vox;

    unsigned int bitIndex = x | (y << 2) | (z << 4);
    uint64_t voxelBit = 1ull << (uint64_t)bitIndex;
    this->bitmask |= voxelBit; // set the bit in the bitmask, indicating that there's a voxel here
}

std::shared_ptr<VoxelData> VoxelSubChunk::get(unsigned int x, unsigned int y, unsigned int z) {
    if (x > 3 || y > 3 || z > 3) return std::shared_ptr<VoxelData>(nullptr);
    return this->data[x][y][z];
}

void VoxelSubChunk::clear(unsigned int x, unsigned int y, unsigned int z) {
    if (x > 3 || y > 3 || z > 3) return; // voxel out of bounds
    this->data[x][y][z].reset();

    unsigned int bitIndex = x | (y << 2) | (z << 4);
    uint64_t voxelBit = 1ull << (uint64_t)bitIndex;
    this->bitmask &= ~voxelBit; // clear the bit in the bitmask, indicating that there's no voxel here
}

void VoxelSubChunk::clear() {
    for (unsigned int x = 0; x < 4; x++)
    for (unsigned int y = 0; y < 4; y++)
    for (unsigned int z = 0; z < 4; z++) {
        data[x][y][z].reset();
    }
    this->bitmask = 0;
}

void VoxelChunk::set(unsigned int x, unsigned int y, unsigned int z, std::shared_ptr<VoxelData> vox) {
    if (x > 15 || y > 15 || z > 15) return; // voxel out of bounds

    unsigned int chX = x >> 2;
    unsigned int chY = y >> 2;
    unsigned int chZ = z >> 2;
    std::shared_ptr<VoxelSubChunk>& sub = this->data[chX][chY][chZ];

    if (!sub) {
        sub = std::make_shared<VoxelSubChunk>();
    }
    sub->set(x % 4, y % 4, z % 4, vox);

    unsigned int bitIndex = chX | (chY << 2) | (chZ << 4);
    uint64_t voxelBit = 1ull << (uint64_t)bitIndex;
    this->bitmask |= voxelBit; // set the bit in the bitmask, indicating that there's a voxel subchunk here
}

void VoxelChunk::clear(unsigned int x, unsigned int y, unsigned int z) {
    if (x > 15 || y > 15 || z > 15) return; // voxel out of bounds

    unsigned int chX = x >> 2;
    unsigned int chY = y >> 2;
    unsigned int chZ = z >> 2;
    std::shared_ptr<VoxelSubChunk> sub = this->data[chX][chY][chZ];

    if (!sub) return; // voxel subchunk doesn't exist
    sub->clear(x % 4, y % 4, z % 4);

    if (sub->getBitmask() == 0) {   // the entire subchunk is empty, we can free it and clear the bit in the bitmask
        sub.reset();

        unsigned int bitIndex = chX | (chY << 2) | (chZ << 4);
        uint64_t voxelBit = 1ull << (uint64_t)bitIndex;
        this->bitmask &= ~voxelBit; // clear the bit in the bitmask, indicating that there's no voxel subchunk here
    }
}

void VoxelChunk::clear() {
    for (unsigned int x = 0; x < 4; x++)
    for (unsigned int y = 0; y < 4; y++)
    for (unsigned int z = 0; z < 4; z++) {
        data[x][y][z].reset();
    }
    this->bitmask = 0;
}

std::shared_ptr<VoxelSubChunk> VoxelChunk::getSubChunk(unsigned int x, unsigned int y, unsigned int z) {
    return this->data[x][y][z];
}
}