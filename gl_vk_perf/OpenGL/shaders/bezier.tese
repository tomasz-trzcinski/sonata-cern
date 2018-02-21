#version 430

layout(isolines) in;
layout(location = 0)uniform mat4 MVP;

layout(location = 0) patch in vec3 cPoint[2];
layout(location = 2) patch in vec3 inColor;

layout(location = 0) out vec3 outColor;

/*
vec3 bezier2(vec3 a, vec3 b, float t) {
	return mix(a, b, t);
}

vec3 bezier3(vec3 a, vec3 b, vec3 c, float t) {
	return mix(bezier2(a, b, t), bezier2(b, c, t), t);
}

vec3 bezier_cubic(vec3 a, vec3 b, vec3 c, vec3 d, float t) {
	return mix(bezier3(a, b, c, t), bezier3(b, c, d, t), t);
}
*/

vec3 bezier_cubic(vec3 a, vec3 b, vec3 c, vec3 d, float t) {
	return (1-t)*(1-t)*(1-t)*a + 3*t*(1-t)*(1-t)*b + 3*t*t*(1-t)*c + t*t*t*d;
}

void main() {
	float t = gl_TessCoord.x;

	vec3 newPos = bezier_cubic(gl_in[0].gl_Position.xyz, cPoint[0], cPoint[1], gl_in[1].gl_Position.xyz, t);
	
	outColor = inColor;
	gl_Position = MVP * vec4(newPos, 1);
}
