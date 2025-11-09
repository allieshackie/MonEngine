#version 460

layout(std140) uniform OverlaySettings
{
    mat4 pvMat;
};

in vec3 position;
in vec4 color;

out vec4 vColor;

void main()
{
	gl_Position = pvMat * vec4(position, 1.0);
	vColor = color;
}