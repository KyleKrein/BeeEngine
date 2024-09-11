#version 460

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 3) in vec2 vTexCoord;
layout (instanced location = 4) in mat4 vModel;

layout(location = 0) out vec2 outTexCoord;

layout(set = 0, binding = 0) uniform UniformBufferCamera
{
    mat4 projView;
} camera;

void main()
{
    outTexCoord = vTexCoord;
    gl_Position = camera.projView * vModel * vec4(vPosition, 1.0);
}