#version 450

layout(location = 0) out vec4 color;
layout(location = 1) out int entityID;

in vec2 v_TexCoord;
in vec4 v_Color;
in flat float v_TextureIndex;
in float v_TilingFactor;
in flat int v_EntityID;

uniform sampler2D u_Textures[16];
void main()
{
    vec4 texColor = v_Color;

    switch(int(v_TextureIndex))
    {
        case  0: texColor *= texture(u_Textures[ 0], v_TexCoord * v_TilingFactor); break;
        case  1: texColor *= texture(u_Textures[ 1], v_TexCoord * v_TilingFactor); break;
        case  2: texColor *= texture(u_Textures[ 2], v_TexCoord * v_TilingFactor); break;
        case  3: texColor *= texture(u_Textures[ 3], v_TexCoord * v_TilingFactor); break;
        case  4: texColor *= texture(u_Textures[ 4], v_TexCoord * v_TilingFactor); break;
        case  5: texColor *= texture(u_Textures[ 5], v_TexCoord * v_TilingFactor); break;
        case  6: texColor *= texture(u_Textures[ 6], v_TexCoord * v_TilingFactor); break;
        case  7: texColor *= texture(u_Textures[ 7], v_TexCoord * v_TilingFactor); break;
        case  8: texColor *= texture(u_Textures[ 8], v_TexCoord * v_TilingFactor); break;
        case  9: texColor *= texture(u_Textures[ 9], v_TexCoord * v_TilingFactor); break;
        case 10: texColor *= texture(u_Textures[10], v_TexCoord * v_TilingFactor); break;
        case 11: texColor *= texture(u_Textures[11], v_TexCoord * v_TilingFactor); break;
        case 12: texColor *= texture(u_Textures[12], v_TexCoord * v_TilingFactor); break;
        case 13: texColor *= texture(u_Textures[13], v_TexCoord * v_TilingFactor); break;
        case 14: texColor *= texture(u_Textures[14], v_TexCoord * v_TilingFactor); break;
        case 15: texColor *= texture(u_Textures[15], v_TexCoord * v_TilingFactor); break;
    }
    color = texColor;

    entityID = v_EntityID;
}