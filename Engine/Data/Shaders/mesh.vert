#version 460

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

layout(std140) uniform FrameSettings 
{
    mat4 projection;
    mat4 view;
};

layout(std140) uniform MeshSettings 
{
    mat4 model;
    float hasTexture;
    float hasBones;
    float gTargetBone;
    vec4 solidColor;
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
out vec4 vBoneDebugColor;

// Colors corresponding to bone IDs
const vec3 boneColors[4] = vec3[](vec3(1.0, 0.0, 0.0),  // Red for Bone 0
                           vec3(0.0, 1.0, 0.0),  // Green for Bone 1
                           vec3(0.0, 0.0, 1.0),  // Blue for Bone 2
                           vec3(1.0, 1.0, 0.0)); // Yellow for Bone 3

vec4 debugShowBoneWeights() 
{
    vec4 weightColor;
    if (gTargetBone != -1) {
        for (int i = 0 ; i < 4; i++) {
            if (boneIds[i] == gTargetBone) {
                if (weights[i] >= 0.7) {
                    weightColor = vec4(1.0, 0.0, 0.0, 1.0) * weights[i];
                } else if (weights[i] >= 0.4 && weights[i] <= 0.6) {
                    weightColor = vec4(0.0, 1.0, 0.0, 1.0) * weights[i];
                } else if (weights[i] >= 0.1) {
                    weightColor = vec4(1.0, 1.0, 0.0, 1.0) * weights[i];
                }
                return vec4(weightColor.xyz, 1.0);
            }
        }
    }
    return vec4(1.0, 0, 0, 1.0);
}

vec4 getAppliedTransform(vec4 value) 
{
    vec4 result = value;
    if (hasBones != 0) {
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
            mat4 boneTransform = boneMatrices[boneIds[i]];
            result += weights[i] * (boneTransform * value);
        }
    }

    return result;
}

void main()
{
    vBoneDebugColor = debugShowBoneWeights();
    vPosition = vec3(getAppliedTransform(vec4(position, 1.0)));
    vNormal = vec3(normalize(getAppliedTransform(vec4(normal, 0.0))));  
	vTexCoord = texCoord;
    vTexCoord.y = 1.0 - vTexCoord.y;
    gl_Position = projection * view * model * vec4(vPosition, 1.0);
}