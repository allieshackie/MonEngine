// GLSL shader version 1.30 (for OpenGL 3.1)
#version 460

layout(std140) uniform SpriteSettings 
{
    mat4 pvmMat;
    mat4 textureTransform;
};

in vec3 position;
in vec2 texCoord;

out vec2 vTexCoord;

void main()
{
    // Transform and pass the vertex's texture coordinate to the fragment shader.
    vec4 newCoords = textureTransform * vec4(texCoord, 1.0, 1.0);
    vTexCoord = vec2(newCoords);
    gl_Position = pvmMat * vec4(position, 1.0);
}