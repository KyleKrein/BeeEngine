#version 450 core

layout (location = 0) in vec3 vPosition;
layout (instanced location = 1) in vec4 vColor;
layout (instanced location = 2) in mat4 vModel;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform UniformBufferCamera
{
    mat4 projView;
} camera;

void main()
{
    outColor = vColor;
    gl_Position = camera.projView * vModel * vec4(vPosition, 1.0);
}