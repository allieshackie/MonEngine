// GLSL texturing shader
#version 140

in vec2 vTexCoord;
in vec4 vColor;

uniform sampler2D textureAtlas;

void main() {
    // Output the glyph color (you can apply additional processing here if needed)
    gl_FragColor = vec4(vColor.rgb, vColor.a * texture(textureAtlas, vTexCoord).a);
}