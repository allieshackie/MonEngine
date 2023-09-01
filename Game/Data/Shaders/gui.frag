// GLSL texturing shader
#version 140

in vec2 fragTexCoords;

uniform sampler2D textureAtlas;

void main() {
    // Sample the texture atlas using the texture coordinates
    vec4 glyphColor = texture(textureAtlas, fragTexCoords);

    // Output the glyph color (you can apply additional processing here if needed)
    gl_FragColor = glyphColor;
}