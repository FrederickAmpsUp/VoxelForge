#version 330 core

#include "voxel-raytracing.glsl"

in vec3 vFragCoord_ws;

out vec4 oFragColor;

uniform usampler3D uChunkData;
uniform usampler3D uSubChunkData;

uniform uvec3 uWorldSize_chunks;

void main() {
    uvec2 originChunk = texture(uChunkData, ivec3(0)).rg;
    vec4 dbg = vec4(texture(uChunkData, ivec3(0)));

    uvec3 subtreeIndex = uvec3(floor(vFragCoord_ws*3.9999));

    bool filled = checkBitmask(originChunk, subtreeIndex);

    oFragColor = vec4(vec3(subtreeIndex)/4.0 * float(!filled), 1.0);
    oFragColor = vec4(vec2(originChunk != uvec2(0)), 0.0, 1.0);
    oFragColor = dbg;
}