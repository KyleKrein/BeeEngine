#version 460

layout (location = 0) in vec3 vPosition;
layout (instanced location = 1) in vec4 vColor;
layout (instanced location = 2) in vec3 vPositionOffset0;
layout (instanced location = 3) in vec3 vPositionOffset1;
layout (instanced location = 4) in vec3 vPositionOffset2;
layout (instanced location = 5) in vec3 vPositionOffset3;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform UniformBufferCamera
{
    mat4 projView;
} camera;

void main()
{
    outColor = vColor;
    vec3 offset = vec3(0.0);
    switch(gl_VertexIndex)
    {
        case 0:
            offset = vPositionOffset0;
            break;
        case 1:
            offset = vPositionOffset1;
            break;
        case 2:
            offset = vPositionOffset2;
            break;
        case 3:
            offset = vPositionOffset3;
            break;
        default:
            offset = vPositionOffset0;
            break;
    }
    gl_Position = camera.projView * vec4(offset, 1.0);
}