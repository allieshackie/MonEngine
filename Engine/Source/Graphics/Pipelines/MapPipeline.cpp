#include "LLGL/Utils/Parse.h"
#include "LLGL/Utils/VertexFormat.h"
#include "LLGL/Utils/Utility.h"

#include "Core/Camera.h"
#include "Graphics/Core/ResourceManager.h"
#include "Graphics/Core/Texture.h"

#include "MapPipeline.h"

void MapPipeline::Render(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat,
                         const Map& map) const
{
	// set graphics pipeline
	commandBuffer.SetPipelineState(*mPipeline);
	commandBuffer.SetVertexBuffer(*mVertexBuffer);

	_Render(commandBuffer, pvMat, map);
}

void MapPipeline::_Render(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat,
                          const Map& map) const
{
	commandBuffer.SetResourceHeap(*mResourceHeap, mWrittenTextureId);
	_UpdateUniforms(commandBuffer, pvMat, map.GetPosition(), map.GetMapSize(),
	                map.GetRotation());
	commandBuffer.Draw(4, 0);
}

void MapPipeline::RenderMapTexture(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat,
                                   const Map& map) const
{
	const auto textureId = ResourceManager::GetTextureId(map.GetTexturePath());
	commandBuffer.SetResourceHeap(*mResourceHeap, textureId);
	_UpdateUniforms(commandBuffer, pvMat, map.GetPosition(), map.GetMapSize(), map.GetRotation());
	commandBuffer.Draw(4, 0);
}

void MapPipeline::_UpdateUniforms(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat, glm::vec3 pos,
                                  glm::vec3 size, glm::vec3 rot) const
{
	// Update
	auto model = glm::mat4(1.0f);
	model = translate(model, pos);
	model = translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
	// Apply rotation in ZXY order
	model = rotate(model, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
	model = rotate(model, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = rotate(model, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));

	model = translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
	model = scale(model, size);

	const auto textureClip = glm::mat4(1.0f);

	const SpriteSettings settings = {pvMat * model, textureClip};
	commandBuffer.UpdateBuffer(*mConstantBuffer, 0, &settings, sizeof(settings));
}

void MapPipeline::_UpdateUniformsModel(LLGL::CommandBuffer& commandBuffer, glm::vec3 pos, glm::vec3 size, glm::vec3 rot,
                                       glm::vec4 texClip) const
{
	// Update
	auto model = glm::mat4(1.0f);
	model = translate(model, pos);
	model = translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
	// Apply rotation in ZXY order
	model = rotate(model, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
	model = rotate(model, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = rotate(model, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));

	model = translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
	model = scale(model, size);

	auto textureClip = glm::mat4(1.0f);
	textureClip = translate(textureClip, glm::vec3(texClip.x, texClip.y, 0.0f));
	textureClip = scale(textureClip, glm::vec3(texClip.z, texClip.w, 1.0f));

	const SpriteSettings settings = {model, textureClip};
	commandBuffer.UpdateBuffer(*mConstantBuffer, 0, &settings, sizeof(settings));
}

void MapPipeline::Init(LLGL::RenderSystemPtr& renderSystem)
{
	mConstantBuffer = renderSystem->CreateBuffer(LLGL::ConstantBufferDesc(sizeof(SpriteSettings)),
	                                             &spriteSettings);

	LLGL::VertexFormat vertexFormat;
	vertexFormat.AppendAttribute({"position", LLGL::Format::RGB32Float});
	vertexFormat.AppendAttribute({"color", LLGL::Format::RGBA32Float});
	vertexFormat.AppendAttribute({"texCoord", LLGL::Format::RG32Float});

	std::string vertPath = SHADERS_FOLDER;
	vertPath.append("sprite.vert");

	std::string fragPath = SHADERS_FOLDER;
	fragPath.append("sprite.frag");

	mShader = std::make_unique<Shader>(*renderSystem, vertexFormat, vertPath.c_str(),
	                                   fragPath.c_str());

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
		pipelineDesc.primitiveTopology = LLGL::PrimitiveTopology::TriangleStrip;

		// Enable depth test and writing
		//pipelineDesc.depth.testEnabled = true;
		//pipelineDesc.depth.writeEnabled = true;
	}
	mPipeline = renderSystem->CreatePipelineState(pipelineDesc);

	_CreateResourceHeap(renderSystem);

	mVertexBuffer = renderSystem->CreateBuffer(
		VertexBufferDesc(static_cast<std::uint32_t>(mVertices.size() * sizeof(Vertex)),
		                 mShader->GetVertexFormat()),
		mVertices.data()
	);
}

void MapPipeline::_CreateResourceHeap(const LLGL::RenderSystemPtr& renderSystem)
{
	// Resource Heap

	// 1st sampler state with default settings
	// Create nearest sampler
	LLGL::SamplerDescriptor samplerDesc;
	{
		samplerDesc.minFilter = LLGL::SamplerFilter::Nearest;
		samplerDesc.magFilter = LLGL::SamplerFilter::Nearest;
		samplerDesc.mipMapFilter = LLGL::SamplerFilter::Nearest;
	}
	const auto sampler = renderSystem->CreateSampler(samplerDesc);

	const auto textures = ResourceManager::LoadAllTexturesFromFolder(renderSystem);

	std::vector<LLGL::ResourceViewDescriptor> resourceViews;
	int addedTexture = mWrittenMapTexture != nullptr ? 1 : 0;
	resourceViews.reserve((textures.size() + addedTexture) * 3);

	for (const auto& texture : textures)
	{
		resourceViews.emplace_back(mConstantBuffer);
		resourceViews.emplace_back(&texture->GetTextureData());
		resourceViews.emplace_back(&texture->GetSamplerData());
	}

	if (mWrittenMapTexture)
	{
		resourceViews.emplace_back(mConstantBuffer);
		resourceViews.emplace_back(mWrittenMapTexture);
		resourceViews.emplace_back(sampler);
		mWrittenTextureId = textures.size() + 1;
	}

	mResourceHeap = renderSystem->CreateResourceHeap(mPipelineLayout, resourceViews);
}

void MapPipeline::WriteQueuedMapTextures(const LLGL::RenderSystemPtr& renderSystem,
                                         LLGL::CommandBuffer& commandBuffer, const Map& map)
{
	_InitMapTexturePipeline(commandBuffer, renderSystem, map);
}

// TODO: Where should this be?  Writing map texture to render
void MapPipeline::_InitMapTexturePipeline(LLGL::CommandBuffer& commandBuffer,
                                          const LLGL::RenderSystemPtr& renderSystem,
                                          const Map& map)
{
	// Create Render Target
	mWrittenMapTexture = renderSystem->CreateTexture(
		LLGL::Texture2DDesc(LLGL::Format::RGBA8UNorm, mTextureSize.width, mTextureSize.height)
	);

	// Create render-target with multi-sampling
	LLGL::RenderTargetDescriptor renderTargetDesc;
	{
		renderTargetDesc.resolution = mTextureSize;
		renderTargetDesc.colorAttachments[0] = mWrittenMapTexture;
	}
	const auto renderTarget = renderSystem->CreateRenderTarget(renderTargetDesc);

	// Render Target Pipeline
	LLGL::GraphicsPipelineDescriptor pipelineDesc;
	{
		pipelineDesc.vertexShader = &mShader->GetVertexShader();
		pipelineDesc.fragmentShader = &mShader->GetFragmentShader();
		pipelineDesc.pipelineLayout = mPipelineLayout;
		pipelineDesc.primitiveTopology = LLGL::PrimitiveTopology::TriangleStrip;
		pipelineDesc.renderPass = renderTarget->GetRenderPass();
		pipelineDesc.viewports = {LLGL::Viewport{{0, 0}, renderTarget->GetResolution()}};

		// Enable depth test and writing
		//pipelineDesc.depth.testEnabled = true;
		//pipelineDesc.depth.writeEnabled = true;
	}

	const auto pipeline = renderSystem->CreatePipelineState(pipelineDesc);
	_WriteMapTexture(commandBuffer, map, pipeline, renderTarget);

	_CreateResourceHeap(renderSystem);
}

void MapPipeline::_WriteMapTexture(LLGL::CommandBuffer& commandBuffer, const Map& map,
                                   LLGL::PipelineState* writePipeline, LLGL::RenderTarget* writeTarget) const
{
	// Set the render target and clear the color buffer
	commandBuffer.BeginRenderPass(*writeTarget);
	{
		// Clear color and depth buffers of active framebuffer (i.e. the render target)
		// TODO: {LLGL::ColorRGBAf{0.2f, 0.7f, 0.1f}} ? Clear with this color when drawing map?
		commandBuffer.Clear(LLGL::ClearFlags::ColorDepth);

		// Bind graphics pipeline for render target
		commandBuffer.SetPipelineState(*writePipeline);
		commandBuffer.SetVertexBuffer(*mVertexBuffer);

		const auto textureId = ResourceManager::GetTextureId(map.GetTexturePath());
		commandBuffer.SetResourceHeap(*mResourceHeap, textureId);

		const auto& mapData = map.GetData();
		for (int i = 0; i < mapData.size(); i++)
		{
			glm::vec3 tilePos;
			glm::vec3 size;
			glm::vec4 clip;
			map.CalculateTileDrawData(i, tilePos, size, clip);

			_UpdateUniformsModel(commandBuffer, tilePos, size, map.GetRotation(), clip);
			commandBuffer.Draw(4, 0);
		}
	}
	commandBuffer.EndRenderPass();

	commandBuffer.GenerateMips(*mWrittenMapTexture);
}
