layout(set = 0, binding = 0) uniform  SceneData{
	mat4 view;
	mat4 proj;
	mat4 viewProj;
	vec4 ambientColor;
	vec4 sunlightDirection; //w for sun power
	vec4 sunlightColor;
} sceneData;

layout(set = 1, binding = 0) uniform MaterialData{
	vec4 colorFactors;
	vec4 metalRoughFactors;
} materialData;

layout(set = 1, binding = 1) uniform texture2D colorTexture;
layout(set = 1, binding = 2) uniform sampler colorSampler;
layout(set = 1, binding = 3) uniform texture2D metalRoughTexture;
layout(set = 1, binding = 4) uniform sampler metalRoughSampler;
