#version 330 core

uniform usampler3D uChunkData;
uniform usampler3D uSubChunkData;
uniform usampler3D uVoxelData;
uniform mat4 uViewMatrix;

uniform uvec3 uWorldSize_chunks;

#include "voxel-raytracing.glsl"

in vec3 vFragCoord_ws;

out vec4 oFragColor;

void main() {
    vec3 rd = normalize(vFragCoord_ws - inverse(uViewMatrix)[3].xyz);
    vec3 ro = vFragCoord_ws;

    bool hit = worldMarch(ro, rd);

    if (!hit) discard;

    VoxelData vox = readVoxelData(ro);

    oFragColor = vec4(vec3(ro) / vec3(uWorldSize_chunks), 1.0);
}