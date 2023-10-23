#include "LLGL/Misc/Utility.h"

#include "Shader.h"

Shader::Shader(LLGL::RenderSystem& renderer, LLGL::VertexFormat vertexFormat, const char* vertexFilePath,
               const char* fragmentFilePath) : mVertexFormat(std::move(vertexFormat))
{
	LoadShaderProgram(renderer, vertexFilePath, fragmentFilePath);
}

LLGL::Shader& Shader::GetVertexShader() const
{
	return *mVertexShader;
}

LLGL::Shader& Shader::GetFragmentShader() const
{
	return *mFragmentShader;
}

const LLGL::VertexFormat& Shader::GetVertexFormat() const
{
	return mVertexFormat;
}

void Shader::LoadShaderProgram(LLGL::RenderSystem& renderer, const char* vertexFilePath, const char* fragmentFilePath)
{
	auto deviceVertShaderDesc = LLGL::ShaderDescFromFile(LLGL::ShaderType::Vertex, vertexFilePath);
	{
		deviceVertShaderDesc.vertex.inputAttribs = mVertexFormat.attributes;
	}

	mVertexShader = renderer.CreateShader(deviceVertShaderDesc);

	auto deviceFragmentShaderDesc = LLGL::ShaderDescFromFile(LLGL::ShaderType::Fragment, fragmentFilePath);

	mFragmentShader = renderer.CreateShader(deviceFragmentShaderDesc);
}
