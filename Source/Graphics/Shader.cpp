#include "Shader.h"

Shader::Shader(LLGL::RenderSystem& renderer, const LLGL::VertexFormat& vertexFormat, const char* vertexFilePath, const char* fragmentFilePath)
{
    LoadShaderProgram(renderer, vertexFormat, vertexFilePath, fragmentFilePath);
}

LLGL::ShaderProgram& Shader::GetShaderProgram() const
{
    return *mShaderProgram;
}

void Shader::LoadShaderProgram(LLGL::RenderSystem& renderer, const LLGL::VertexFormat& vertexFormat, const char* vertexFilePath, const char* fragmentFilePath)
{
    std::string fullVertPath = SHADER_PATH + vertexFilePath;
    std::string fullFragPath = SHADER_PATH + fragmentFilePath;
    LLGL::ShaderDescriptor vertShaderDesc = { LLGL::ShaderType::Vertex, fullVertPath.c_str() };
    LLGL::ShaderDescriptor fragShaderDesc = { LLGL::ShaderType::Fragment, fullFragPath.c_str() };

    vertShaderDesc.vertex.inputAttribs = vertexFormat.attributes;

    LLGL::Shader* vertShader = renderer.CreateShader(vertShaderDesc);
    LLGL::Shader* fragShader = renderer.CreateShader(fragShaderDesc);

    LLGL::ShaderProgramDescriptor shaderProgramDesc;
    {
        shaderProgramDesc.vertexShader = vertShader;
        shaderProgramDesc.fragmentShader = fragShader;
    }
    mShaderProgram = renderer.CreateShaderProgram(shaderProgramDesc);
}
