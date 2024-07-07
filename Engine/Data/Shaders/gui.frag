// GLSL texturing shader
#version 140

in vec2 vTexCoord;
in vec4 vColor;

out vec4 fragColor;

uniform sampler2D textureAtlas;

void main() {
    // Output the glyph color (you can apply additional processing here if needed)
    fragColor = vec4(vColor.rgb, vColor.a * texture(textureAtlas, vTexCoord).a);
}