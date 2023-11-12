// GLSL shader version 1.30 (for OpenGL 3.1)
#version 460

layout(std140) uniform GUISettings 
{
    mat4 pvmMat;
};

in vec2 guiPosition;
in vec2 guiTexCoord;
in vec4 color;

out vec2 vTexCoord;
out vec4 vColor;

void main() {
    // Pass the texture coordinates to the fragment shader
    vTexCoord = guiTexCoord;
    vColor = color;

    // Transform the vertex position
    gl_Position = pvmMat * vec4(guiPosition.x, guiPosition.y, 0.0, 1.0);
}