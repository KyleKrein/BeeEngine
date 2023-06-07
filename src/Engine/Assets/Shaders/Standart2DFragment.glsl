#version 450 core

layout(location = 0) out vec4 color;
in vec2 v_TexCoord;
in vec4 v_Color;
in flat float v_TextureIndex;
in float v_TilingFactor;

uniform sampler2D u_Textures[16];
void main()
{
    color = texture(u_Textures[int(v_TextureIndex)], v_TexCoord * v_TilingFactor) * v_Color;
}