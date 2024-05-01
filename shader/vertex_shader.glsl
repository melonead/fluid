#version 440
layout (location=0) in vec3 position;
layout (location=1) in vec2 offset;
layout (location=2) in float velocitySqrd;

out vec2 pos;
out vec2 disp;
out float velSqrd;


uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

mat4 createTranslation(float x, float y, float z);

float mult = 3;


void main()
{	
	pos = position.xy;
	disp = offset;
	velSqrd = velocitySqrd;
	mat4 transl = createTranslation(offset.x, offset.y, 0.0);
	mat4 mv_matrix = mv_matrix * transl;
	gl_Position = proj_matrix * mv_matrix * vec4(position.x, position.y, position.z, 1.0);
}

mat4 createTranslation(float x, float y, float z)
{
	mat4 trans = mat4 (
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		x,   y,   z,   1.0
	);

	return trans;
}
