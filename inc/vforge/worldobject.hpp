#pragma once

#include <fglw/fglw.hpp>

namespace voxelforge {

class WorldObject {
public:
    virtual void draw(fglw::RenderTarget& fb, glm::mat4 view, glm::mat4 proj) = 0;
};
}