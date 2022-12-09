#include "Shader.h"

Shader::Shader(LLGL::RenderSystem& renderer, LLGL::VertexFormat vertexFormat, const char* vertexFilePath,
               const char* fragmentFilePath) : mVertexFormat(std::move(vertexFormat))
{
	LoadShaderProgram(renderer, vertexFilePath, fragmentFilePath);
}

LLGL::ShaderProgram& Shader::GetShaderProgram() const
{
	return *mShaderProgram;
}

const LLGL::VertexFormat& Shader::GetVertexFormat() const
{
	return mVertexFormat;
}

void Shader::LoadShaderProgram(LLGL::RenderSystem& renderer, const char* vertexFilePath, const char* fragmentFilePath)
{
	std::string fullVertPath = SHADER_PATH + vertexFilePath;
	std::string fullFragPath = SHADER_PATH + fragmentFilePath;
	LLGL::ShaderDescriptor vertShaderDesc = {LLGL::ShaderType::Vertex, fullVertPath.c_str()};
	LLGL::ShaderDescriptor fragShaderDesc = {LLGL::ShaderType::Fragment, fullFragPath.c_str()};

	vertShaderDesc.vertex.inputAttribs = mVertexFormat.attributes;

	LLGL::Shader* vertShader = renderer.CreateShader(vertShaderDesc);
	// Print info log (warnings and errors)
	std::string vertLog = vertShader->GetReport();
	if (!vertLog.empty())
	{
		std::cerr << vertLog << std::endl;
	}
	LLGL::Shader* fragShader = renderer.CreateShader(fragShaderDesc);
	// Print info log (warnings and errors)
	std::string fragLog = fragShader->GetReport();
	if (!fragLog.empty())
	{
		std::cerr << fragLog << std::endl;
	}

	LLGL::ShaderProgramDescriptor shaderProgramDesc;
	{
		shaderProgramDesc.vertexShader = vertShader;
		shaderProgramDesc.fragmentShader = fragShader;
	}
	mShaderProgram = renderer.CreateShaderProgram(shaderProgramDesc);
	// Link shader program and check for errors
	if (mShaderProgram->HasErrors())
	{
		std::cout << mShaderProgram->GetReport() << std::endl;
		//throw std::runtime_error(mShaderProgram->GetReport());
	}
}
