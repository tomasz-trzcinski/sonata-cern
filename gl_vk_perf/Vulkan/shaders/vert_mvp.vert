#version 460

layout(location = 0) in vec3 inVert;
layout(location = 0) out vec3 outColor;

layout(push_constant) uniform PCBlock
{
	mat4 mvp;
	vec3 color;
} pbo;

void main()
{
	outColor = pbo.color;
	gl_Position = pbo.mvp * vec4(inVert, 1.0);
}
