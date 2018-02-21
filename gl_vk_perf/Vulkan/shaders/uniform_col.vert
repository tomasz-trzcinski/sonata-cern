#version 460

layout(push_constant) uniform PCColor
{
	vec3 color;
} pbo;

layout(location = 0) in vec3 inVert;
layout(location = 0) out vec3 outColor;

void main()
{
	outColor = pbo.color;
	gl_Position = vec4(inVert, 1.0);
}