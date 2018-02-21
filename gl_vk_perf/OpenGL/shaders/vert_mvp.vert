#version 430

layout(location = 0) in vec3 inVert;
layout(location = 0)uniform mat4 MVP;

void main()
{
	gl_Position = MVP * vec4(inVert, 1.0);
}