#version 450 core

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;
layout(location = 1) out float outEntityID;

/*TODO: convert strings like layout(set = 1, binding = 0) uniform sampler2D u_sampler
to lines down below automatically. also need to replace all usings of sampler2D with
sampler2D(u_texture, u_sampler)*/
layout(set = 1, binding = 0) uniform texture2D u_texture;
layout(set = 1, binding = 1) uniform sampler u_sampler;
layout(set = 1, binding = 2) uniform texture2D u_idTexture;
layout(set = 1, binding = 3) uniform sampler u_idSampler;

void main()
{
    outEntityID = texture(sampler2D(u_idTexture, u_idSampler), fragTexCoord).r;
    outColor = texture(sampler2D(u_texture, u_sampler), fragTexCoord);
    if(outColor.a == 0.0f)
        discard;
}