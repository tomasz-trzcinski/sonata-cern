#version 430

layout(location = 2) uniform vec3 color;

layout(location = 0) in vec3 inVert;
layout(location = 0) out vec3 outColor;

void main()
{
	outColor = color;
	gl_Position = vec4(inVert, 1.0);
}