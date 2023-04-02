#version 460

layout(std140) uniform SpriteSettings 
{
    mat4 pvmMat;
    mat4 textureTransform;
};

uniform sampler2D colorMap;

in vec2 vTexCoord;

out vec4 fragColor;

void main()
{    
    fragColor = texture(colorMap, vTexCoord);
}  