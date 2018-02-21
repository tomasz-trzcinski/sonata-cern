#version 460

layout(location = 0) in vec3 inVert;
layout(location = 0) out vec3 outColor;

layout(std430, binding = 0) buffer color_data
{
	vec3 colors[];
};

void main()
{
	outColor = colors[gl_DrawID];
	gl_Position = vec4(inVert, 1.0);
}