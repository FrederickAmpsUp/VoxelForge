#version 330 core

uniform usampler3D uChunkData;
uniform usampler3D uSubChunkData;
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

    vec3 green1 = vec3(0.1, 1.0, 0.1);
    vec3 green2 = vec3(0.0, 1.0, 0.3);

    oFragColor = vec4(mix(green1, green2, ro.y) * ro.y, 1.0);
}