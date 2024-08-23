#version 460

const int MAX_BONES = 40;
const int MAX_BONE_INFLUENCE = 4;

layout(std140) uniform Settings 
{
    mat4 model;
    mat4 view;
    mat4 projection;
    mat4 textureClip;
    vec3 viewPos;
    int numLights;
    mat4 boneMatrices[MAX_BONES];
};

in vec3 position;
in vec3 normal;
in vec2 texCoord;
in ivec4 boneIds; // ivec4 is int vector
in vec4 weights;

out vec3 vPosition;
out vec3 vNormal;
out vec2 vTexCoord;

void main()
{
    vec3 transformedPosition = position;
    vec3 transformedNormal = normal;

    // mat4 boneTransform = weights[0] * boneMatrices[boneIds[0]];
    // boneTransform += weights[1] * boneMatrices[boneIds[1]];
    // boneTransform += weights[2] * boneMatrices[boneIds[2]];
    // boneTransform += weights[3] * boneMatrices[boneIds[3]];
    
    // transformedPosition = vec3(boneTransform * vec4(position, 1.0));
    // transformedNormal = mat3(transpose(inverse(boneTransform))) * normal;
    
    vNormal = transformedNormal;  
	vTexCoord = texCoord;

	vPosition = vec3(model * vec4(transformedPosition, 1.0));
    gl_Position = projection * view * vec4(vPosition, 1.0);
}