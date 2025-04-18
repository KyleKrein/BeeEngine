#version 450 core

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec4 fragColor;
layout(location = 0) out vec4 finalColor;
layout(location = 1) out float finalEntity;

void main()
{
    finalColor = fragColor;
    finalEntity = 0;
    if (finalColor.a == 0)
    {
        discard;
    }
}
