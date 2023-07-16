#version 450

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in float fragTilingFactor;

layout(location = 0) out vec4 outColor;

/*TODO: convert strings like layout(set = 1, binding = 0) uniform sampler2D u_sampler
to lines down below automatically. also need to replace all usings of sampler2D with
sampler2D(u_texture, u_sampler)*/
layout(binding = 1) uniform texture2D u_texture;
layout(binding = 2) uniform sampler u_sampler;

void main()
{
    vec4 texColor = fragColor * texture(sampler2D(u_texture, u_sampler), fragTexCoord * fragTilingFactor);
    outColor = texColor;
}