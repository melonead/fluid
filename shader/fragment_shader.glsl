#version 440

// in vec4 color;
out vec4 color;
in vec2 pos;
in vec2 disp;
in float velSqrd;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
float radius = 0.2;
vec3 red = vec3(1.0, 0.0, 0.0);
vec3 blue = vec3(0.0, 0.0, 1.0);
float fade = 0.05;
vec3 lerp(vec3 a, vec3 b, float c);

void main()
{
	
	float trans;
	//vec4 col = vec4(0.0, 0.0, 0.0, 0.0);
	float d = length(pos);
	vec3 col = vec3(smoothstep(radius, radius - fade, d));
	trans = 1.0 - step(radius, d);
	//col *= mix(blue, red, velSqrd);
	col *= lerp(blue, red, velSqrd);
	color = vec4(col, trans);
}

vec3 lerp(vec3 a, vec3 b, float c) {
	return a * (1.0 - c) + b * c;
}