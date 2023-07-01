#version 460

layout(std140) uniform VolumeSettings
{
    mat4 pvmMat;
    vec4 color;
};

layout(location = 2) uniform mat4 projection;
layout(location = 3) uniform mat4 view;

in vec3 position;
in vec3 normal;

out vec3 vNormal;

void main()
{
	gl_Position = pvmMat * vec4(position, 1.0);
    vNormal = (model * vec4(normal, 0)).xyz;
}