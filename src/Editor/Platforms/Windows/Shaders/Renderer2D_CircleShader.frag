#version 450 core

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in float fragThickness;
layout(location = 3) in float fragFade;
layout(location = 4) in flat int fragEntityID;

layout(location = 0) out vec4 outColor;
layout(location = 1) out float outEntityID;

void main()
{
    float distance = 1.0 - length(fragTexCoord * 2.0 - 1.0);
    float circleAlpha = smoothstep(0.0, fragFade, distance);
    circleAlpha *= smoothstep(fragThickness + fragFade, fragThickness, distance);
    outColor = fragColor;
    outColor.a *= circleAlpha;
    outEntityID = fragEntityID;
    if(outColor.a == 0.0f)
        discard;
}