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
	float a = 1.597, b = 0.07, c = 0.37;
	
	vec3 p0 = gl_in[0].gl_Position.xyz;
	vec3 p1 = gl_in[1].gl_Position.xyz;
	vec3 p2 = gl_in[2].gl_Position.xyz;
	vec3 p3 = gl_in[3].gl_Position.xyz;
	
	vec3 W1 = normalize(p2-p0);	
	vec3 W2 = normalize(p3-p1);
	
	vec3 delta = p2 - p1;
	float atan_delta = atan(delta.y, delta.x);
	
	float theta = atan(W1.y, W1.x) - atan_delta;
	float phi   = atan_delta - atan(W2.y, W2.x);
	
	float st = sin(theta);
	float ct = cos(theta);
	float sp = sin(phi);
	float cp = cos(phi);
	
	float d = distance(p2, p1);
	
	float alpha = a*(st-b*sp)*(sp-b*st)*(ct-cp);
	float rho   = (2+alpha)/(1+(1-c)*ct+c*cp);
	float sigma = (2-alpha)/(1+(1-c)*cp+c*ct);
	
	float tau = 1;
	
    if(gl_InvocationID == 0)
    {
		setTesselationLevels(p1, p2);
		
		cPoint[0] = p1 + (rho/(3*tau)) * d * W1;
		outColor = inColor[0];
    }
	else
	{
		cPoint[1] = p2 - (sigma/(3*tau)) * d * W2;
	}

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID+1].gl_Position;
}
