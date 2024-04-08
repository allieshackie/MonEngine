#include "LLGL/Utils/Parse.h"
#include "LLGL/Utils/Utility.h"

#include "Graphics/Core/ResourceManager.h"

#include "PipelineBase.h"

PipelineBase::PipelineBase(const LLGL::RenderSystemPtr& renderSystem, const char* vertexFile, const char* fragmentFile,
                           LLGL::PrimitiveTopology topology)
{
	_InitConstantBuffer(renderSystem);
	_InitShader(renderSystem, vertexFile, fragmentFile);
	_InitPipeline(renderSystem, topology);
	_InitResourceHeap(renderSystem);
}

void PipelineBase::_InitConstantBuffer(const LLGL::RenderSystemPtr& renderSystem)
{
	mConstantBuffer = renderSystem->CreateBuffer(LLGL::ConstantBufferDesc(sizeof(Settings)), &settings);
}

void PipelineBase::_InitShader(const LLGL::RenderSystemPtr& renderSystem, const char* vertexFile,
                               const char* fragmentFile)
{
	LLGL::VertexFormat vertexFormat;
	vertexFormat.AppendAttribute({"position", LLGL::Format::RGB32Float});
	vertexFormat.AppendAttribute({"normal", LLGL::Format::RGB32Float});
	vertexFormat.AppendAttribute({"texCoord", LLGL::Format::RG32Float});

	std::string vertPath = SHADERS_FOLDER;
	vertPath.append(vertexFile);

	std::string fragPath = SHADERS_FOLDER;
	fragPath.append(fragmentFile);

	mShader = std::make_unique<Shader>(*renderSystem, vertexFormat, vertPath.c_str(),
	                                   fragPath.c_str());
}

void PipelineBase::_InitPipeline(const LLGL::RenderSystemPtr& renderSystem, LLGL::PrimitiveTopology topology)
{
	// All layout bindings that will be used by graphics and compute pipelines
	// Create pipeline layout
	mPipelineLayout = renderSystem->CreatePipelineLayout(
		LLGL::Parse("heap{cbuffer(0):vert:frag, texture(0):frag, sampler(0):frag}"));

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
		pipelineDesc.depth.testEnabled = true;
		pipelineDesc.depth.writeEnabled = true;
	}
	mPipeline = renderSystem->CreatePipelineState(pipelineDesc);
}

void PipelineBase::_InitResourceHeap(const LLGL::RenderSystemPtr& renderSystem)
{
	// Resource Heap
	const auto textures = ResourceManager::LoadAllTexturesFromFolder(renderSystem);

	std::vector<LLGL::ResourceViewDescriptor> resourceViews;
	resourceViews.reserve(textures.size() * 3);

	for (const auto& texture : textures)
	{
		resourceViews.emplace_back(mConstantBuffer);
		resourceViews.emplace_back(&texture->GetTextureData());
		resourceViews.emplace_back(&texture->GetSamplerData());
	}

	mResourceHeap = renderSystem->CreateResourceHeap(mPipelineLayout, resourceViews);
}
