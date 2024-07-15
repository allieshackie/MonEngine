// GLSL shader version 1.30 (for OpenGL 3.1)
#version 460

layout(std140) uniform Settings 
{
    mat4 model;
    mat4 view;
    mat4 projection;
    mat4 textureClip;
};

in vec3 position;
in vec3 normal;
in vec2 texCoord;

out vec2 vTexCoord;

void main()
{
    // Transform and pass the vertex's texture coordinate to the fragment shader.
    vec4 newCoords = textureClip * vec4(texCoord, 1.0, 1.0);
    vTexCoord = vec2(newCoords);
    gl_Position = projection * view * model * vec4(position, 1.0);
}