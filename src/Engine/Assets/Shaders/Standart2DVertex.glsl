#version 450 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TextureIndex;
layout(location = 4) in float a_TilingFactor;

uniform mat4 u_ViewProjection;

out vec2 v_TexCoord;
out vec4 v_Color;
out flat float v_TextureIndex;
out float v_TilingFactor;
void main()
{
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    v_TextureIndex = a_TextureIndex;
    v_TilingFactor = a_TilingFactor;
    gl_Position =  u_ViewProjection * vec4(a_Position, 1.0);
}