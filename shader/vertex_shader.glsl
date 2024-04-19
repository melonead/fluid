#version 440
layout (location=0) in vec3 position;
layout (location=1) in vec2 offset;


uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

// out vec4 color;

mat4 createTranslation(float x, float y, float z);

float mult = 3;


void main()
{	
	mat4 transl = createTranslation(offset.x, offset.y, 0.0);
	mat4 mv_matrix = mv_matrix * transl;
	// color = vec4(cos(gl_InstanceID * mult), sin(gl_InstanceID * mult), tan(gl_InstanceID * mult), 1.0); 
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
