#include <vforge/world.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace voxelforge {

VoxelWorld::VoxelWorld(unsigned int dX, unsigned int dY, unsigned int dZ) : chunks({}) {
    this->dim = glm::uvec3(dX, dY, dZ);

    this->chunkData = fglw::Texture3D(dX, dY, dZ, GL_RG32UI);
    this->subChunkData = fglw::Texture3D(dX  * 4, dY * 4, dZ * 4, GL_RG32UI);
    this->voxelData = fglw::Texture3D(dX * 16, dY * 16, dZ * 16, GL_RGBA32UI);

        // cube vertices
    const std::vector<VertexLayout> vertices = {
        {{-0.5f, -0.5f,  0.5f}},  // Vertex 0
        {{0.5f, -0.5f,  0.5f}},  // Vertex 1
        {{0.5f,  0.5f,  0.5f}},  // Vertex 2
        {{-0.5f,  0.5f,  0.5f}},  // Vertex 3

        {{-0.5f, -0.5f, -0.5f}},  // Vertex 4
        {{0.5f, -0.5f, -0.5f}},  // Vertex 5
        {{0.5f,  0.5f, -0.5f}},  // Vertex 6
        {{-0.5f,  0.5f, -0.5f}}   // Vertex 7
    };

    const std::vector<unsigned int> indices = {
        0, 1, 2,
        2, 3, 0,

        5, 4, 7,
        7, 6, 5,

        4, 0, 3,
        3, 7, 4,

        1, 5, 6,
        6, 2, 1,

        3, 2, 6,
        6, 7, 3,

        4, 5, 1,
        1, 0, 4
    };

    this->meshRenderer = fglw::TriangleMesh<VertexLayout>(vertices, indices);

    this->modelMatrix = glm::identity<glm::mat4>();
    this->modelMatrix = glm::scale(this->modelMatrix, glm::vec3(dX, dY, dZ));
    this->modelMatrix = glm::translate(this->modelMatrix, glm::vec3(0.5, 0.5, 0.5));

    this->voxelRTShader = fglw::ShaderProgram::loadGLSLFiles("shaders/voxel-world-raytrace.vsh", "shaders/voxel-world-raytrace.fsh");

    this->voxelRTShader.uniform("uModelMatrix", this->modelMatrix);

    this->voxelRTShader.uniform("uChunkData", this->chunkData);
    this->voxelRTShader.uniform("uSubChunkData", this->subChunkData);

    this->ready = false;
}
VoxelWorld::VoxelWorld(glm::uvec3 dim) : VoxelWorld(dim.x, dim.y, dim.z) { }

void VoxelWorld::rebuild() {
    if (this->ready) return;
    this->ready = true;

    std::vector<uint64_t> chunkDataBuf(this->dim.x * this->dim.y * this->dim.z, -1);
    std::vector<uint64_t> subChunkDataBuf(this->dim.x * this->dim.y * this->dim.z * 4 * 4 * 4, 0);

    for (const auto& [position, chunk] : this->chunks) {
        size_t offset = position.x + position.y*this->dim.x + position.z*this->dim.x*this->dim.y;

        if (chunk) { // should always be true, but be safe!
            chunkDataBuf[offset] = chunk->getBitmask();

            FGLW_DEBUG_PRINTF("chunkBitmask: %lu, offset: %u\n", chunk->getBitmask(), offset);

            for (size_t i = 0; i < 4; ++i)
            for (size_t j = 0; j < 4; ++j)
            for (size_t k = 0; k < 4; ++k) {
                const auto& subChunk = chunk->getSubChunk(i, j, k);
                if (!subChunk) continue;

                glm::uvec3 scPosition = position * 4u + glm::uvec3(i, j, k);
                FGLW_DEBUG_PRINTF("scPosition: %u %u %u\n", scPosition.x, scPosition.y, scPosition.z);
                size_t scOffset = scPosition.x + scPosition.y*this->dim.x*4u + scPosition.z*this->dim.x*this->dim.y*4u*4u;

                subChunkDataBuf[scOffset] = subChunk->getBitmask();
            }
            
        }
    }

    FGLW_DEBUG_PRINTF("%lu\n", chunkDataBuf[0]);
    this->chunkData.upload(chunkDataBuf.data());
    //this->subChunkData.upload(subChunkDataBuf.data());
}

void VoxelWorld::draw(fglw::RenderTarget& fb, glm::mat4 view, glm::mat4 proj) {
    this->rebuild(); // re-upload data if neccesary
    this->voxelRTShader.uniform("uViewMatrix", view);
    this->voxelRTShader.uniform("uProjectionMatrix", proj);

    this->meshRenderer.draw(fb, this->voxelRTShader);
}

void VoxelWorld::set(glm::uvec3 position, std::shared_ptr<voxelforge::VoxelData> vox) {
    auto& chunk = this->chunks[position / 16u]; // will create a nullptr chunk if one doesn't exist at this location

    if (!chunk) chunk = std::make_shared<voxelforge::VoxelChunk>();

    chunk->set(position, vox);

    this->ready = false;
}
}