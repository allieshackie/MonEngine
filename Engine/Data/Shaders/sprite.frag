#version 460

layout(std140) uniform Settings 
{
    mat4 model;
    mat4 view;
    mat4 projection;
    mat4 textureClip;
    vec3 lightPos; 
    float padding1; // Padding for alignment
    vec3 viewPos; 
    float padding2; // Padding for alignment
    vec3 lightColor;
    float padding3; // Padding for alignment
};

uniform sampler2D colorMap;

in vec2 vTexCoord;

out vec4 fragColor;

void main()
{    
    fragColor = texture(colorMap, vTexCoord);
}  