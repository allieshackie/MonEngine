#pragma once
#include "LLGL/Misc/VertexFormat.h"
#include <LLGL/LLGL.h>
#include <LLGL/Misc/VertexFormat.h>

using GLuint = unsigned int;
static std::string SHADER_PATH = "D:/dev/MonDev/Data/Shaders/";

class Shader
{
public:
	Shader(LLGL::RenderSystem& renderer, LLGL::VertexFormat vertexFormat, const char* vertexFilePath,
	       const char* fragmentFilePath);

	~Shader();

	LLGL::Shader& GetVertexShader() const;
	LLGL::Shader& GetFragmentShader() const;

	const LLGL::VertexFormat& GetVertexFormat() const;

private:
	void LoadShaderProgram(LLGL::RenderSystem& renderer, const char* vertexFilePath, const char* fragmentFilePath);

	LLGL::Shader* mVertexShader = nullptr;
	LLGL::Shader* mFragmentShader = nullptr;

	LLGL::VertexFormat mVertexFormat;
};
