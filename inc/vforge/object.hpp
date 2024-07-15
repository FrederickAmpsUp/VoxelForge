#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <vforge/worldobject.hpp>
#include <vforge/internal.hpp>
#include <vforge/chunk.hpp>
#include <memory>

namespace voxelforge {

class VoxelObject : public WorldObject {
public:
    VoxelObject(unsigned int dX, unsigned int dY, unsigned int dZ, glm::mat4x4 modelMatrix = glm::mat4x4(1.0f));
    VoxelObject(glm::uvec3 dim, glm::mat4x4 modelMatrix = glm::mat4x4(1.0f));

    void rebuild();

    void set(glm::uvec3 position, std::shared_ptr<voxelforge::VoxelData> vox);
    void clear();

    void setMaterial(uint32_t index, glm::vec4 material);

    virtual void draw(fglw::RenderTarget& fb, glm::mat4 view, glm::mat4 proj) override;

    glm::uvec3 size() const { return this->dim; }

protected:
    struct VertexLayout {
        glm::vec3 aPosition;

        static const fglw::VertexAttributeLayout layout() {
            return fglw::VertexAttributeLayout::empty()
                .add(GL_FLOAT, 3); // aPosition
        }
    };

private:
    glm::uvec3 dim;
    std::unordered_map<glm::uvec3, std::shared_ptr<voxelforge::VoxelChunk>, internal::uvec3Hash> chunks;
    fglw::Texture3D chunkData;
    fglw::Texture3D subChunkData;
    fglw::Texture3D voxelData;
    fglw::Texture1D materialData;
    std::array<glm::vec4, 256> materials;

    fglw::TriangleMesh<VertexLayout> meshRenderer;
    fglw::ShaderProgram voxelRTShader;

    glm::mat4x4 modelMatrix;

    bool ready = false;
    std::unordered_map<glm::uvec3, std::shared_ptr<voxelforge::VoxelChunk>, internal::uvec3Hash> modificationCache;
};
}