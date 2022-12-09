#pragma once
#include <LLGL/LLGL.h>

using GLuint = unsigned int;
static std::string SHADER_PATH = "../Data/Shaders/";

class Shader
{
public:
	Shader(LLGL::RenderSystem& renderer, LLGL::VertexFormat vertexFormat, const char* vertexFilePath,
	       const char* fragmentFilePath);

	LLGL::ShaderProgram& GetShaderProgram() const;

	const LLGL::VertexFormat& GetVertexFormat() const;

private:
	void LoadShaderProgram(LLGL::RenderSystem& renderer, const char* vertexFilePath, const char* fragmentFilePath);

	LLGL::ShaderProgram* mShaderProgram = nullptr;
	LLGL::VertexFormat mVertexFormat;
};
