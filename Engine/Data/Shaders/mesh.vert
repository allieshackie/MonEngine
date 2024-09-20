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
    mat4 textureClip;
    vec4 hasBones; // [0] = hasBones, [1] = gTargetBone (for debugging)
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
out vec4 vTestColor;

void main()
{
    if (hasBones[0] != 0 && boneIds[0] != -1) {
        mat4 boneTransform = weights[0] * boneMatrices[boneIds[0]];
        if (boneIds[1] != -1) {
            boneTransform += weights[1] * boneMatrices[boneIds[1]];
        }
        if (boneIds[2] != -1) {
            boneTransform += weights[2] * boneMatrices[boneIds[2]];
        }
        if (boneIds[3] != -1) {
            boneTransform += weights[3] * boneMatrices[boneIds[3]];
        }
        
	    vPosition = vec3(model * (boneTransform * vec4(position, 1.0)));
        vNormal = normalize(mat3(transpose(inverse(boneTransform))) * normal);  
        
        //DEBUG BONE WEIGHT 
        bool found = false;
        /*
        vPosition = vec3(model * vec4(position, 1.0));
        vNormal = mat3(transpose(inverse(model))) * normal;  
        
        vec4 weightColor;
        for (int i = 0 ; i < 4; i++) {
            if (boneIds[i] == hasBones[1]) {
                if (weights[i] >= 0.7) {
                    weightColor = vec4(1.0, 0.0, 0.0, 1.0) * weights[i];
                } else if (weights[i] >= 0.4 && weights[i] <= 0.6) {
                    weightColor = vec4(0.0, 1.0, 0.0, 1.0) * weights[i];
                } else if (weights[i] >= 0.1) {
                    weightColor = vec4(1.0, 1.0, 0.0, 1.0) * weights[i];
                }
                vTestColor = vec4(weightColor.xyz, 1.0);
                found = true;
                break;
            }
        }
        */

        if (!found) {
            vTestColor = vec4(0.5, 0.5, 0.5, 1.0);
        }
    }
    else {
        vPosition = vec3(model * vec4(position, 1.0));
        vNormal = mat3(transpose(inverse(model))) * normal;  
        vTestColor = vec4(1.0, 0, 0, 1.0);
    }

	vTexCoord = texCoord;
    gl_Position = projection * view * vec4(vPosition, 1.0);
}