#version 450

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vColor;

layout(location = 0) out vec3 outColor;

layout(push_constant) uniform PushConstants {
    mat4 renderMatrix;
} pushConstants;

void main() {
    outColor = vColor;
    gl_Position = pushConstants.renderMatrix * vec4(vPosition, 1.0);
}