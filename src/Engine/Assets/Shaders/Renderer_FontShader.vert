#version 450 core
layout (location = 0) in vec2 vPosition;

layout (instanced location = 1) in vec2 vTexCoord0;
layout (instanced location = 2) in vec2 vTexCoord1;
layout (instanced location = 3) in vec2 vTexCoord2;
layout (instanced location = 4) in vec2 vTexCoord3;
layout (instanced location = 5) in vec2 vPositionOffset0;
layout (instanced location = 6) in vec2 vPositionOffset1;
layout (instanced location = 7) in vec2 vPositionOffset2;
layout (instanced location = 8) in vec2 vPositionOffset3;
layout (instanced location = 9) in vec4 vForegroundColor;
layout (instanced location = 10) in vec4 vBackgroundColor;
//layout (instanced location = 10) in int vEntityID;

layout(location = 0) out vec4 outForegroundColor;
layout(location = 1) out vec4 outBackgroundColor;
layout(location = 2) out vec2 outTexCoord;
//layout(location = 3) out flat int outEntityID;

layout(set = 0, binding = 0) uniform UniformBufferCamera
{
    mat4 projView;
} camera;

void main()
{
    outForegroundColor = vForegroundColor;
    outBackgroundColor = vBackgroundColor;
    vec2 offset = vec2(0.0);
    switch(gl_VertexIndex)
    {
        case 0:
        outTexCoord = vTexCoord0;
        offset = vPositionOffset0;
        break;
        case 1:
        outTexCoord = vTexCoord1;
        offset = vPositionOffset1;
        break;
        case 2:
        outTexCoord = vTexCoord2;
        offset = vPositionOffset2;
        break;
        case 3:
        outTexCoord = vTexCoord3;
        offset = vPositionOffset3;
        break;
        default:
        outTexCoord = vTexCoord0;
        offset = vPositionOffset0;
        break;
    }

    gl_Position = camera.projView * vec4(offset, 0.0, 1.0);
}