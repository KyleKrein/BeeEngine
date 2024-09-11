#version 460

#extension GL_ARB_gpu_shader_int64 : require

layout(location = 0) in vec4 fragColor;

layout(location = 0) out vec4 outColor;
layout(location = 1) out uint64_t outEntityID;

void main()
{
    outColor = fragColor;
    outEntityID = 0;
}