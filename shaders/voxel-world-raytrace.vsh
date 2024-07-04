#version 330 core

                        // model-space position
layout(location = 0) in vec3 aPosition;

uniform mat4x4 uModelMatrix;
uniform mat4x4 uViewMatrix;
uniform mat4x4 uProjectionMatrix;

out vec3 vFragCoord_ws;

void main() {
    mat4x4 pvmMatrix = uProjectionMatrix * uViewMatrix * uModelMatrix;

    vFragCoord_ws = (uModelMatrix * vec4(aPosition, 1.0)).xyz;

    gl_Position = pvmMatrix * vec4(aPosition, 1.0);
}