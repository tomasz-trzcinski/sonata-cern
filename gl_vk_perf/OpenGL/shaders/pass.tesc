#version 430

layout(vertices = 2) out;

layout(location = 1) uniform vec3 camPos;

layout(location = 0) in vec3 inColor[];

layout(location = 0) patch out vec3 cPoint[2];
layout(location = 2) patch out vec3 outColor;

void setTesselationLevels(in vec3 p1, in vec3 p2)
{
	gl_TessLevelOuter[0] = 1;
	
	float d = distance(camPos, p1);
	float d2 = distance(camPos, p2);
	
	if(d2 < d)
		d = d2;
		
	//float level = smoothstep(1.0, 10.0, d) * 63.0 + 1.0;
	//gl_TessLevelOuter[1] = level;
	
	gl_TessLevelOuter[1] = 8;
}

void main()
{
	if(gl_InvocationID == 0)
	{
		vec3 p1 = gl_in[0].gl_Position.xyz;
		vec3 p2 = gl_in[3].gl_Position.xyz;
		setTesselationLevels(p1, p2);
		
		cPoint[0] = gl_in[1].gl_Position.xyz;
		outColor = inColor[0];
	}
	else
	{
		cPoint[1] = gl_in[2].gl_Position.xyz;
	}

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID*3].gl_Position;
}
