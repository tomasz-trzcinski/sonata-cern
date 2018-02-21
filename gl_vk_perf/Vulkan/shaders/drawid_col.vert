#version 460

layout(std430, binding = 1) buffer color_data
{
	vec3 colors[];
};

layout(location = 0) in vec3 inVert;
layout(location = 0) out vec3 outColor;

void main()
{
	outColor = colors[gl_DrawID];
	gl_Position = vec4(inVert, 1.0);
}