#version 460

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragUV;
layout(location = 2) in flat int fragEntityID;

layout(location = 0) out vec4 outColor;
layout(location = 1) out float outEntityID;

void main()
{
    outColor = fragColor;
    outEntityID = float(fragEntityID);
}