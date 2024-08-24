#version 460

const int MAX_BONES = 40;
const int MAX_BONE_INFLUENCE = 4;

layout(std140) uniform MeshSettings 
{
    mat4 model;
    mat4 view;
    mat4 projection;
    mat4 textureClip;
    vec4 hasBones;
};

layout(std430) buffer BoneBuffer
{
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
    if (hasBones[0] != 0) {
        mat4 boneTransform = weights[0] * boneMatrices[boneIds[0]];
        boneTransform += weights[1] * boneMatrices[boneIds[1]];
        boneTransform += weights[2] * boneMatrices[boneIds[2]];
        boneTransform += weights[3] * boneMatrices[boneIds[3]];
        
        vec3 transformedPosition = vec3(boneTransform * vec4(position, 1.0));
        vec3 transformedNormal = mat3(transpose(inverse(boneTransform))) * normal;
	    vPosition = vec3(model * vec4(transformedPosition, 1.0));
        vNormal = transformedNormal;  
    }
    else {
        vPosition = vec3(model * vec4(position, 1.0));
        vNormal = mat3(transpose(inverse(model))) * normal;  
    }

	vTexCoord = texCoord;
    gl_Position = projection * view * vec4(vPosition, 1.0);
}