#pragma once
#include <LLGL/LLGL.h>

typedef unsigned int GLuint;
static std::string SHADER_PATH = "D:/dev/MonDev/Data/Shaders/";

class Shader
{
public:
	Shader(const LLGL::VertexFormat& vertexFormat, const char* vertexFilePath, const char* fragmentFilePath);

	LLGL::ShaderProgram& GetShaderProgram() const;
	
private:
    void LoadShaderProgram(const LLGL::VertexFormat& vertexFormat, const char* vertexFilePath, const char* fragmentFilePath);

    LLGL::ShaderProgram* mShaderProgram = nullptr;
};