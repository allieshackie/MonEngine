#include "Graphics/Core/ResourceManager.h"

#include "PipelineBase.h"

void PipelineBase::InitShader(const LLGL::RenderSystemPtr& renderSystem, const LLGL::VertexFormat& vertexFormat,
                              const char* vertexFile, const char* fragmentFile)
{
	std::string vertPath = SHADERS_FOLDER;
	vertPath.append(vertexFile);

	std::string fragPath = SHADERS_FOLDER;
	fragPath.append(fragmentFile);

	mShader = std::make_unique<Shader>(*renderSystem, vertexFormat, vertPath.c_str(),
	                                   fragPath.c_str());
}

void PipelineBase::InitPipeline(const LLGL::RenderSystemPtr& renderSystem,
                                const LLGL::PipelineLayoutDescriptor& pipelineLayoutDesc,
                                LLGL::PrimitiveTopology topology, bool enableDepthTest)
{
	// All layout bindings that will be used by graphics and compute pipelines
	// Create pipeline layout
	mPipelineLayout = renderSystem->CreatePipelineLayout(pipelineLayoutDesc);

	// Create graphics pipeline
	LLGL::GraphicsPipelineDescriptor pipelineDesc;
	{
		pipelineDesc.vertexShader = &mShader->GetVertexShader();
		pipelineDesc.fragmentShader = &mShader->GetFragmentShader();
		pipelineDesc.pipelineLayout = mPipelineLayout;
		pipelineDesc.primitiveTopology = topology;
		pipelineDesc.blend.targets[0].blendEnabled = true;
		pipelineDesc.rasterizer.cullMode = LLGL::CullMode::Disabled;

		// Enable depth test and writing
		pipelineDesc.depth.testEnabled = enableDepthTest;
		pipelineDesc.depth.writeEnabled = enableDepthTest;
	}
	mPipeline = renderSystem->CreatePipelineState(pipelineDesc);
}

void PipelineBase::InitResourceHeap(const LLGL::RenderSystemPtr& renderSystem,
                                    const std::vector<LLGL::ResourceViewDescriptor>& resourceViews)
{
	mResourceHeap = renderSystem->CreateResourceHeap(mPipelineLayout, resourceViews);
}
