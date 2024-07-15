#version 460

layout(std140) uniform Settings 
{
    mat4 model;
    mat4 view;
    mat4 projection;
    mat4 textureClip;
};

in vec3 position;
in vec3 normal;
in vec2 texCoord;

out vec3 vPosition;
out vec3 vNormal;
out vec2 vTexCoord;

void main()
{
	vPosition = vec3(model * vec4(position, 1.0));
    vNormal = mat3(transpose(inverse(model))) * normal;  
    
    gl_Position = projection * view * vec4(vPosition, 1.0);
	vTexCoord = texCoord;
}