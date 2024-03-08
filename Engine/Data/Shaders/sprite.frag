#version 460

layout(std140) uniform Settings 
{
    mat4 pvmMat;
    mat4 textureClip;
    mat4 mMat;
};

uniform sampler2D colorMap;

in vec2 vTexCoord;

out vec4 fragColor;

void main()
{    
    fragColor = texture(colorMap, vTexCoord);
}  