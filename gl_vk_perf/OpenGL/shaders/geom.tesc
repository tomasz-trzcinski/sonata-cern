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

void calcControlPoints(in vec3 p0, in vec3 p1, in vec3 p2, out vec3 c0, out vec3 c1)
{
	float t = 0.3;
	
	float d01 = distance(p0, p1);
	float d12 = distance(p1, p2);
	
	float fa = 0, fb = 0;
	if((d01+d12) != 0)
	{
		fa = t * d01/(d01+d12);
		fb = t * d12/(d01+d12);
	}
	
	c0 = p1 + fa*(p0-p2);
	c1 = p1 - fb*(p0-p2);
}

void main()
{
	vec3 p1 = gl_in[1].gl_Position.xyz;
	vec3 p2 = gl_in[2].gl_Position.xyz;

    if(gl_InvocationID == 0)
    {
		setTesselationLevels(p1, p2);
		
		vec3 p0 = gl_in[0].gl_Position.xyz;
		vec3 dummy;
		calcControlPoints(p0, p1, p2, dummy, cPoint[0]);
		
		outColor = inColor[0];
    }
	else
	{
		vec3 p3 = gl_in[3].gl_Position.xyz;
		vec3 dummy;
		calcControlPoints(p1, p2, p3, cPoint[1], dummy);
	}

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID+1].gl_Position;
}
