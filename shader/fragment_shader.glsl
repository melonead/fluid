#version 440

// in vec4 color;
out vec4 newcolor;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;


void main()
{
	newcolor = vec4(0.0, 1.0, 0.0, 1.0);
}