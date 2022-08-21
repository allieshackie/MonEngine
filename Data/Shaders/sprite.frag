#version 140

uniform sampler2D colorMap;

in vec2 vTexCoord;

out vec4 fragColor;

void main()
{    
    fragColor = texture(colorMap, vTexCoord);
}  