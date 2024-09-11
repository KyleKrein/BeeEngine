#version 460

layout(location = 0) in vec4 fForegroundColor;
layout(location = 1) in vec4 fBackgroundColor;
layout(location = 2) in vec2 fTexCoord;
layout(location = 3) in flat uvec2 fEntityID;

layout(location = 0) out vec4 outColor;
layout(location = 1) out uvec2 outEntityID;


layout(set = 1, binding = 0) uniform texture2D u_texture;
layout(set = 1, binding = 1) uniform sampler u_sampler;

float screenPxRange()
{
    const float pxRange = 2.0;
    vec2 unitRange = vec2(pxRange)/vec2(textureSize(sampler2D(u_texture, u_sampler), 0));
    vec2 screenTexSize = vec2(1.0)/fwidth(fTexCoord);
    return max(0.5f * dot(unitRange, screenTexSize), 1.0f);
}

float median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

void main()
{
    vec3 msd = texture(sampler2D(u_texture, u_sampler), fTexCoord).rgb;
    float sd = median(msd.r, msd.g, msd.b);
    float screenPxDistance = screenPxRange() * (sd - 0.5f);
    float opacity = clamp(screenPxDistance + 0.5f, 0.0f, 1.0f);

    outColor = mix(fBackgroundColor, fForegroundColor, opacity);
    outEntityID = fEntityID;
    if(outColor.a == 0.0f)
        discard;
}