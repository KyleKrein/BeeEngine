#version 450 core

layout(location = 0) in vec4 fragColor;

layout(location = 0) out vec4 outColor;
layout(location = 1) out float outEntityID;

void main()
{
    outColor = fragColor;
    outEntityID = 0;
}