#version 440
out vec4 color;
in vec2 pos;

void main()
{	

	float thickness = 0.5;
	float fade = 0.005;
	float radius = 0.05;


	float d = radius - length(vec2(pos));
	vec3 col = vec3(smoothstep(0.0, fade, d));
	col *= vec3(radius - smoothstep(thickness - fade, thickness, d));
	if (radius != 0.0)
	{
		col *= vec3(1.0 / radius, 1.0 / radius, 0.1 / radius);
	} else {
		col *= vec3(1.0, 1.0, 0.1);
	}
	
	color = vec4(col, 1.0);
}
