#version 330

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(set = 1, binding = 0) uniform texture2D u_texture;
layout(set = 1, binding = 1) uniform sampler u_sampler;
layout (location = 0) in vec2 fragTexCoord;
layout (location = 1) in vec4 fragColor;

layout (location = 0) out vec4 finalColor;

void main() {
	vec4 texColor = texture(sampler2D(u_texture, u_sampler), fragTexCoord);
	finalColor = fragColor * texColor;
}