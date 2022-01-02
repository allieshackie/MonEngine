#pragma once
#include <glm/glm.hpp>
#include <LLGL/LLGL.h>

typedef unsigned int GLuint;
static std::string SHADER_PATH = "C:/dev/MonDev/Data/Shaders/";

class Shader
{
public:
	Shader(LLGL::RenderSystem& renderer, const LLGL::VertexFormat& vertexFormat, const char* vertexFilePath, const char* fragmentFilePath);

	LLGL::ShaderProgram& GetShaderProgram() const;
	
private:
    void LoadShaderProgram(LLGL::RenderSystem& renderer, const LLGL::VertexFormat& vertexFormat, const char* vertexFilePath, const char* fragmentFilePath);

    LLGL::ShaderProgram* mShaderProgram = nullptr;
};