#version 460

// Fragment input from the vertex shader
in vec4 vertexColor;

// Fragment output color
out vec4 fragColor;

// Fragment shader main function
void main()
{
	fragColor = vertexColor;
}