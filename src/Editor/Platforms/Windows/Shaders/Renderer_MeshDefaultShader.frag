#version 460

#extension GL_GOOGLE_include_directive : require

#include "InputStructures.glsl"

layout(location = 0) in vec3 inNormal;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inUV;
layout(location = 3) in flat int inEntityID;

layout(location = 0) out vec4 outColor;
layout(location = 1) out float outEntityID;

void main()
{
    float lightValue = max(dot(inNormal, sceneData.sunlightDirection.xyz), 0.1f);
    vec3 color = inColor * texture(sampler2D(colorTexture, colorSampler),inUV).xyz;
    vec3 ambient = color * sceneData.ambientColor.xyz;

    outColor = vec4(color * lightValue * sceneData.sunlightColor.w + ambient, 1.0f);
    outEntityID = float(inEntityID);
}