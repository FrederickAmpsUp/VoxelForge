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

class VoxelWorld : public WorldObject {
public:
    VoxelWorld(unsigned int dX, unsigned int dY, unsigned int dZ);
    VoxelWorld(glm::uvec3 dim);

    void rebuild();

    void set(glm::uvec3 position, std::shared_ptr<voxelforge::VoxelData> vox);
    void clear();

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

    fglw::TriangleMesh<VertexLayout> meshRenderer;
    fglw::ShaderProgram voxelRTShader;

    glm::mat4x4 modelMatrix;

    bool ready = false;
};
}