#version 330 core

uniform usampler3D uChunkData;
uniform usampler3D uSubChunkData;
uniform usampler3D uVoxelData;
uniform sampler1D uMaterialData;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uModelMatrix;

uniform uvec3 uWorldSize_chunks;

#include "voxel-raytracing.glsl"

in vec3 vFragCoord_ws;
in vec3 vFragCoord_os;

out vec4 oFragColor;

void main() {
    vec3 cameraOrigin = inverse(uViewMatrix)[3].xyz;
    vec3 objectOrigin = inverse(uModelMatrix)[3].xyz;
    vec3 rd = normalize(vFragCoord_ws - cameraOrigin);
        // ray origin at the surface of the box for optimization and space translation purposes
    vec3 ro = (vFragCoord_os + 0.5) * uWorldSize_chunks;

    bool hit = worldMarch(ro, rd);

    //oFragColor = vec4(vFragCoord_ws, 1.0);
    //return;
    if (!hit) discard;

    VoxelData vox = readVoxelData(ro);

    vec4 c = texelFetch(uMaterialData, vox.matID, 0);

    vec3 hit_ws = (uModelMatrix * vec4(ro - (vec3(uWorldSize_chunks) * 0.5), 1.0)).xyz;
    oFragColor = vec4(c.rgb, 1.0);

    vec4 clipPos = uProjectionMatrix * uViewMatrix * vec4(hit_ws, 1.0);
    clipPos /= clipPos.w;
    gl_FragDepth = (clipPos.z + 1.0) * 0.5;

    //oFragColor = vec4(vec3((((clipPos.z + 1.0) * 0.5) - 4.0) / 4.0), 1.0);
}