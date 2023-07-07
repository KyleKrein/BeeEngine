#version 450

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vColor;

layout(location = 0) out vec3 outColor;

//layout(push_constant) uniform PushConstants {
//    mat4 renderMatrix;
//} pushConstants;
void main() {
    outColor = vColor;
    //const array of positions for the triangle
    //const vec3 positions[3] = vec3[3](
    //    vec3(1.f,1.f, 0.0f),
    //    vec3(-1.f,1.f, 0.0f),
    //    vec3(0.f,-1.f, 0.0f)
    //);
    gl_Position = vec4(vPosition, 1.0);//vec4(positions[gl_VertexIndex], 1.0f);//pushConstants.renderMatrix * vec4(vPosition, 1.0);
}