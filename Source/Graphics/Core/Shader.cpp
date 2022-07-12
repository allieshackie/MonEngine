#include "RendererInstance.h"

#include "Shader.h"

Shader::Shader(const LLGL::VertexFormat& vertexFormat, const char* vertexFilePath, const char* fragmentFilePath)
{
    LoadShaderProgram(vertexFormat, vertexFilePath, fragmentFilePath);
}

LLGL::ShaderProgram& Shader::GetShaderProgram() const
{
    return *mShaderProgram;
}

void Shader::LoadShaderProgram(const LLGL::VertexFormat& vertexFormat, const char* vertexFilePath, const char* fragmentFilePath)
{
    const auto renderer = RendererInstance::GetInstance();
    std::string fullVertPath = SHADER_PATH + vertexFilePath;
    std::string fullFragPath = SHADER_PATH + fragmentFilePath;
    LLGL::ShaderDescriptor vertShaderDesc = { LLGL::ShaderType::Vertex, fullVertPath.c_str() };
    LLGL::ShaderDescriptor fragShaderDesc = { LLGL::ShaderType::Fragment, fullFragPath.c_str() };

    vertShaderDesc.vertex.inputAttribs = vertexFormat.attributes;

    LLGL::Shader* vertShader = renderer->GetRendererSystem()->CreateShader(vertShaderDesc);
    // Print info log (warnings and errors)
    std::string vertLog = vertShader->GetReport();
    if (!vertLog.empty())
    {   
        std::cerr << vertLog << std::endl;
    }
    LLGL::Shader* fragShader = renderer->GetRendererSystem()->CreateShader(fragShaderDesc);
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
    mShaderProgram = renderer->GetRendererSystem()->CreateShaderProgram(shaderProgramDesc);
    // Link shader program and check for errors
    if (mShaderProgram->HasErrors())
    {   
        throw std::runtime_error(mShaderProgram->GetReport());
    }
}
