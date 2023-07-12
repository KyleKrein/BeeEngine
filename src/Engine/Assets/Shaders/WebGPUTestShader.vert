#version 450

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vColor;
layout (instanced location = 3) in mat4 vModel;

layout(location = 0) out vec3 outColor;

layout(binding = 0) uniform UniformBufferCamera
{
    mat4 projView;
} camera;

void main()
{
    outColor = vColor;
    //gl_Position = camera.projView * vModel * vec4(vPosition, 1.0);
    gl_Position = vModel * vec4(vPosition, 1.0);
}