#version 460

layout(location = 0) in vec3 position;
layout(location = 1) in float uv_x;
layout(location = 2) in vec3 normal;
layout(location = 3) in float uv_y;
layout(location = 4) in vec4 color;

layout (instanced location = 5) in mat4 model;
layout (instanced location = 9) in int entityID;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec2 outUV;
layout(location = 2) out int outEntityID;

layout(set = 0, binding = 0) uniform UniformBufferCamera
{
    mat4 projView;
} camera;

void main()
{
    gl_Position = camera.projView * model * vec4(position, 1.0);
    outColor = color;
    outUV = vec2(uv_x, uv_y);
    outEntityID = entityID;
}