#version 440

// in vec4 color;
out vec4 color;
in vec2 pos;
in vec2 disp;
in float velSqrd;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

void main()
{
	float thickness = 0.5;
	float fade = 0.005;
	float radius = 0.05;
	vec4 col = vec4(0.0, 0.0, 0.0, 0.0);
	float d = length(pos);
	float velColor = velSqrd/100.0;
	float r = smoothstep(0.0, 0.5, velColor);
	float g = smoothstep(0.5, 1.0, velColor);
	float b = 1.0;
	if (d < 0.2)
	{
		//col.x = 1.0;
		color = vec4(r, g, b, 1.0);
	}else {
		//col.w = 0.0;
		color = vec4(0.0, 0.0, 0.0, 0.0);
	}
		
	//vec3 col = vec3(smoothstep(0.0, fade, d));
	//col *= vec3(smoothstep(thickness - fade, thickness, d));
	//col.z = 1.0;
	
	//color = vec4(col);
}