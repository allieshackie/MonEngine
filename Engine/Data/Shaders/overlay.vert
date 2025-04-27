#version 460

in vec2 position;
in vec4 color;

out vec4 vColor;

void main()
{
	gl_Position = vec4(position, 1.0, 1.0);
	vColor = color;
}