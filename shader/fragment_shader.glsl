#version 440

// in vec4 color;
out vec4 color;
in vec2 pos;
in vec2 disp;
in float velSqrd;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
float radius = 0.2;

void main()
{
	float thickness = 0.5;
	float fade = 0.05;
	
	float trans;
	//vec4 col = vec4(0.0, 0.0, 0.0, 0.0);
	float d = length(pos);
	float velColor = velSqrd/100.0;
	float r = smoothstep(0.0, 0.5, velColor);
	float g = smoothstep(0.5, 1.0, velColor);
	vec3 col = vec3(smoothstep(radius, radius - fade, d));
	trans = 1.0 - step(radius, d);
	col *= vec3(r, g, 1.0);
	color = vec4(col, trans);
}