#version 460

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

// Colors corresponding to bone IDs
const vec3 boneColors[4] = vec3[](vec3(1.0, 0.0, 0.0),  // Red for Bone 0
                           vec3(0.0, 1.0, 0.0),  // Green for Bone 1
                           vec3(0.0, 0.0, 1.0),  // Blue for Bone 2
                           vec3(1.0, 1.0, 0.0)); // Yellow for Bone 3

layout(std140) uniform MeshSettings 
{
    mat4 model;
    mat4 view;
    mat4 projection;
    vec4 meshFlags; // [0] = hasTexture, [1] = hasBones, [2] = gTargetBone (for debugging)
    vec3 solidColor;
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

void debugShowBoneWeights(bool found) 
{
    vPosition = vec3(model * vec4(position, 1.0));
    vNormal = mat3(transpose(inverse(model))) * normal;  
    
    vec4 weightColor;
    for (int i = 0 ; i < 4; i++) {
        if (boneIds[i] == meshFlags[2]) {
            if (weights[i] >= 0.7) {
                weightColor = vec4(1.0, 0.0, 0.0, 1.0) * weights[i];
            } else if (weights[i] >= 0.4 && weights[i] <= 0.6) {
                weightColor = vec4(0.0, 1.0, 0.0, 1.0) * weights[i];
            } else if (weights[i] >= 0.1) {
                weightColor = vec4(1.0, 1.0, 0.0, 1.0) * weights[i];
            }
            vBoneDebugColor = vec4(weightColor.xyz, 1.0);
            found = true;
            break;
        }
    }
}

vec3 applyBoneTransform(vec4 pos) 
{
    vec3 result = vec3(0.0);
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
        if (boneIds[i] != -1) {
            mat4 boneTransform = boneMatrices[boneIds[i]];
            result += vec3(weights[i] * (boneTransform * pos));
        }
    }

    return result;
}

void main()
{
    if (meshFlags[1] != 0) {
        vec3 transformedPos = applyBoneTransform(vec4(position, 1.0));
        vec3 transformedNormal = normalize(applyBoneTransform(vec4(normal, 0.0)));
        
	    vPosition = vec3(model * vec4(transformedPos, 1.0));
        vNormal = normalize(mat3(transpose(inverse(model))) * transformedNormal); 
        
        //DEBUG BONE WEIGHT 
        bool found = false;
        //debugShowBoneWeights(found);

        if (!found) {
            vBoneDebugColor = vec4(0.5, 0.5, 0.5, 1.0);
        }
    }
    else {
        vPosition = vec3(model * vec4(position, 1.0));
        vNormal = mat3(transpose(inverse(model))) * normal;  
        vBoneDebugColor = vec4(1.0, 0, 0, 1.0);
    }

	vTexCoord = texCoord;
    gl_Position = projection * view * vec4(vPosition, 1.0);
}