#version 460

layout (constant_id = 0) const int LINE_MODEL = 0;

layout(vertices = 2) out;

layout(location = 0) in vec3 inColor[];

layout(location = 0) patch out vec3 cPoint[2];
layout(location = 2) patch out vec3 outColor;

void setTesselationLevels(in vec3 p1, in vec3 p2)
{
	gl_TessLevelOuter[0] = 1;
	
	//float d = distance(camPos, p1);
	//float d2 = distance(camPos, p2);
	
	//if(d2 < d)
	//	d = d2;
		
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
	
	outColor = inColor[0];
	
	switch(LINE_MODEL)
	{
		case 0: // Passthrough
			if(gl_InvocationID == 0) // p1 and p2 are control points in this case
			{
				vec3 p0 = gl_in[0].gl_Position.xyz;
				vec3 p3 = gl_in[3].gl_Position.xyz;
				setTesselationLevels(p0, p3);
				cPoint[0] = p1;
			}
			else
			{
				cPoint[1] = p2;
			}
			gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID*3].gl_Position;
			break;
		
		case 1: // Geometric
			if(gl_InvocationID == 0)
			{
				setTesselationLevels(p1, p2);
				
				vec3 p0 = gl_in[0].gl_Position.xyz;
				vec3 dummy;
				calcControlPoints(p0, p1, p2, dummy, cPoint[0]);
			}
			else
			{
				vec3 p3 = gl_in[3].gl_Position.xyz;
				vec3 dummy;
				calcControlPoints(p1, p2, p3, cPoint[1], dummy);
			}
			gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID+1].gl_Position;
			break;
		
		case 2: // Hobby
			float a = 1.597, b = 0.07, c = 0.37;
	
			vec3 p0 = gl_in[0].gl_Position.xyz;
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
			}
			else
			{
				cPoint[1] = p2 - (sigma/(3*tau)) * d * W2;
			}
			gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID+1].gl_Position;
			break;
	}
}
