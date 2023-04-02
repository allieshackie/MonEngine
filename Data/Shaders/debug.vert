#version 460

layout(std140) uniform Settings
{
    mat4 pvMat;
};

// Vertex attributes (these names must match our vertex format attributes)
in vec3 position;
in vec3 color;

// Vertex output to the fragment shader
out vec3 vertexColor;

// Vertex shader main function
void main()
{
	gl_Position = pvMat * vec4(position, 1.0);
	vertexColor = color;
}