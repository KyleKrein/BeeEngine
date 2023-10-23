#version 450 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 3) in vec2 vTexCoord;
layout (instanced location = 4) in mat4 vModel;
layout (instanced location = 8) in vec4 vColor;
layout (instanced location = 9) in float vTilingFactor;
layout (instanced location = 10) in int vEntityID;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec2 outTexCoord;
layout(location = 2) out float outTilingFactor;
layout(location = 3) out int outEntityID;

layout(set = 0, binding = 0) uniform UniformBufferCamera
{
    mat4 projView;
} camera;

void main()
{
    outColor = vColor;
    outTexCoord = vTexCoord;
    outTilingFactor = vTilingFactor;
    outEntityID = vEntityID;
    gl_Position = camera.projView * vModel * vec4(vPosition, 1.0);
    //gl_Position = vModel * vec4(vPosition, 1.0);
}