#version 460

layout(std140) uniform SpriteSettings 
{
    mat4 model;
    mat4 view;
    mat4 projection;
    mat4 textureClip;
};
uniform sampler2D colorMap;

in vec2 vTexCoord;

out vec4 fragColor;

void main()
{    
    fragColor = texture(colorMap, vTexCoord);
}  