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
    vec4 totalPosition = vec4(0.0);
    mat4 animatedModelMatrix = mat4(0.0);
    for(int i = 0; i < MAX_BONE_INFLUENCE; i++) {
        if (boneIds[i] == -1) {
            continue;
        }
        if (boneIds[i] >= MAX_BONES) {
            totalPosition = vec4(position, 1.0);
            vNormal = mat3(transpose(inverse(model))) * normal;  
            break;
        }

        vec4 localPosition = boneMatrices[boneIds[i]] * vec4(position, 1.0);
        totalPosition += localPosition * weights[i];
        animatedModelMatrix += weights[i] * boneMatrices[boneIds[i]];
    }

    animatedModelMatrix = modelMatrix * animatedModelMatrix;

    vNormal = mat3(transpose(inverse(animatedModelMatrix))) * normal;  
	vTexCoord = texCoord;

	vPosition = vec3(model * vec4(totalPosition, 1.0));
    gl_Position = projection * view * vec4(vPosition, 1.0);
}