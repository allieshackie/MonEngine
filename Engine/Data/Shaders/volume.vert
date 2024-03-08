#version 460

layout(std140) uniform VolumeSettings
{
	mat4 pvmMat;
	mat4 mMat;
};

in vec3 position;
in vec3 normal;
in vec2 texCoord;

out vec3 vNormal;
out vec2 vTexCoord;

void main()
{
	gl_Position = pvmMat * vec4(position, 1);
    vNormal = normalize((mMat * vec4(normal, 0)).xyz);
	vTexCoord = texCoord;
}