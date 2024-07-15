#version 330 core

                        // model-space position
layout(location = 0) in vec3 aPosition;

uniform mat4x4 uModelMatrix;
uniform mat4x4 uViewMatrix;
uniform mat4x4 uProjectionMatrix;
uniform uvec3 uWorldSize_chunks;

out vec3 vFragCoord_ws;
out vec3 vFragCoord_os;

void main() {
    mat4x4 pvmMatrix = uProjectionMatrix * uViewMatrix * uModelMatrix;

    vFragCoord_os = aPosition;
    vec3 aPosCorrected = aPosition * vec3(uWorldSize_chunks);
    vFragCoord_ws = (uModelMatrix * vec4(aPosCorrected, 1.0)).xyz;

    gl_Position = pvmMatrix * vec4(aPosCorrected, 1.0);
}