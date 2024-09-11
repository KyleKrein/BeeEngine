#version 460

#extension GL_GOOGLE_include_directive : require
#extension GL_ARB_gpu_shader_int64 : require

#include "InputStructures.glsl"

struct Vertex {

	vec3 position;
	float uv_x;
	vec3 normal;
	float uv_y;
	vec4 color;
};

//layout(location = 0) in Vertex vertex;

layout(location = 0) in vec3 position;
layout(location = 1) in float uv_x;
layout(location = 2) in vec3 normal;
layout(location = 3) in float uv_y;
layout(location = 4) in vec4 color;

layout (instanced location = 5) in mat4 model;
layout (instanced location = 9) in uint64_t entityID;

layout(location = 0) out vec3 outNormal;
layout(location = 1) out vec3 outColor;
layout(location = 2) out vec2 outUV;
layout(location = 3) out uint64_t outEntityID;

void main()
{
    gl_Position = sceneData.viewProj * model * vec4(position, 1.0);
    outNormal = (model * vec4(normal, 0.0f)).xyz;
    outColor = color.xyz * materialData.colorFactors.xyz;
    outUV = vec2(uv_x, uv_y);
    outEntityID = entityID;
}