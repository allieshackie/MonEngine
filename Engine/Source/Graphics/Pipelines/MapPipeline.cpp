#include "LLGL/Misc/VertexFormat.h"
#include "LLGL/Misc/Utility.h"

#include "Core/Camera.h"
#include "Graphics/Core/ResourceManager.h"
#include "Graphics/Core/Texture.h"
#include "Entity/EntityRegistry.h"
#include "Map/Map.h"

#include "MapPipeline.h"

// TODO: Should we be rendering more than 1 map at a time?  Sublevels/chunks?
void MapPipeline::Render(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat,
                         const std::shared_ptr<Map>& map) const
{
	// set graphics pipeline
	commandBuffer.SetPipelineState(*mPipeline);
	commandBuffer.SetVertexBuffer(*mVertexBuffer);

	_Render(commandBuffer, pvMat, map);
}

void MapPipeline::_Render(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat,
                          const std::shared_ptr<Map>& map) const
{
	commandBuffer.SetResourceHeap(*mResourceHeap, map->GetMapTextureId());
	_UpdateUniforms(commandBuffer, pvMat, map->GetPosition(), map->GetMapSize(),
	                map->GetRotation());
	commandBuffer.Draw(4, 0);
}

void MapPipeline::RenderMapTexture(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat,
                                   const std::shared_ptr<Map>& map) const
{
	// TODO: Is this the correct texture ID? (Map Texture vs the texture used to create the map)
	//commandBuffer.SetResourceHeap(*mResourceHeap, map->GetMapTextureId());
	_UpdateUniforms(commandBuffer, pvMat, map->GetPosition(), map->GetMapSize(), map->GetRotation());
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

void MapPipeline::Init(std::shared_ptr<LLGL::RenderSystem>& renderSystem)
{
	mConstantBuffer = renderSystem->CreateBuffer(LLGL::ConstantBufferDesc(sizeof(SpriteSettings)),
	                                             &spriteSettings);

	LLGL::VertexFormat vertexFormat;
	vertexFormat.AppendAttribute({"position", LLGL::Format::RGB32Float});
	vertexFormat.AppendAttribute({"color", LLGL::Format::RGB32Float});
	vertexFormat.AppendAttribute({"texCoord", LLGL::Format::RG32Float});

	std::string vertPath = SHADERS_FOLDER;
	vertPath.append("sprite.vert");

	std::string fragPath = SHADERS_FOLDER;
	fragPath.append("sprite.frag");

	mShader = std::make_unique<Shader>(*renderSystem, vertexFormat, vertPath.c_str(),
	                                   fragPath.c_str());

	// All layout bindings that will be used by graphics and compute pipelines
	// Create pipeline layout
	mPipelineLayout = renderSystem->CreatePipelineLayout(LLGL::PipelineLayoutDesc(
		"cbuffer(0):vert:frag, texture(0):frag, sampler(0):frag"));

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


	// TODO: This should be separated, when map is opened we'll need to have some sort of Render Context level of callback
	//mMapSystem.RegisterOnCreateCallback([=](const std::shared_ptr<Map>& map) { QueueWriteMapTexture(map); });

	// Callback only usable when editor is active, not for in-game use
	//mMapSystem.RegisterOnUpdateCallback([=]() { QueueWriteMapTexture(mMapSystem.GetCurrentMap()); });
}

void MapPipeline::_CreateResourceHeap(const std::shared_ptr<LLGL::RenderSystem>& renderSystem)
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

	LLGL::ResourceHeapDescriptor resourceHeapDesc;
	{
		resourceHeapDesc.pipelineLayout = mPipelineLayout;
		resourceHeapDesc.resourceViews.reserve((textures.size() + mMapTextures.size()) * 3);

		for (const auto& texture : textures)
		{
			resourceHeapDesc.resourceViews.emplace_back(mConstantBuffer);
			resourceHeapDesc.resourceViews.emplace_back(&texture->GetTextureData());
			resourceHeapDesc.resourceViews.emplace_back(&texture->GetSamplerData());
		}


		const int i = static_cast<int>(textures.size());
		for (const auto& mapTexture : mMapTextures)
		{
			resourceHeapDesc.resourceViews.emplace_back(mConstantBuffer);
			resourceHeapDesc.resourceViews.emplace_back(mapTexture.second);
			resourceHeapDesc.resourceViews.emplace_back(sampler);
		}
	}
	mResourceHeap = renderSystem->CreateResourceHeap(resourceHeapDesc);
}

void MapPipeline::QueueWriteMapTexture(const std::shared_ptr<Map>& map)
{
	mQueuedMaps.emplace_back(map);
}

void MapPipeline::WriteQueuedMapTextures(const std::shared_ptr<LLGL::RenderSystem>& renderSystem,
                                         LLGL::CommandBuffer& commandBuffer)
{
	if (mQueuedMaps.empty()) return;

	for (auto& map : mQueuedMaps)
	{
		_InitMapTexturePipeline(commandBuffer, renderSystem, map);
	}

	mQueuedMaps.clear();
}

// TODO: Where should this be?  Writing map texture to render
void MapPipeline::_InitMapTexturePipeline(LLGL::CommandBuffer& commandBuffer,
                                          const std::shared_ptr<LLGL::RenderSystem>& renderSystem,
                                          std::shared_ptr<Map>& map)
{
	// Create Render Target
	const auto renderTargetTex = renderSystem->CreateTexture(
		LLGL::Texture2DDesc(LLGL::Format::RGBA8UNorm, mTextureSize.width, mTextureSize.height)
	);

	// Create render-target with multi-sampling
	LLGL::RenderTargetDescriptor renderTargetDesc;
	{
		renderTargetDesc.resolution = mTextureSize;

		renderTargetDesc.attachments =
		{
			LLGL::AttachmentDescriptor{LLGL::AttachmentType::Depth},
			LLGL::AttachmentDescriptor{LLGL::AttachmentType::Color, renderTargetTex}
		};
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
	_CreateResourceHeap(renderSystem);

	_WriteMapTexture(commandBuffer, map, pipeline, renderTarget, renderTargetTex);
}

void MapPipeline::_WriteMapTexture(LLGL::CommandBuffer& commandBuffer, const std::shared_ptr<Map>& map,
                                   LLGL::PipelineState* writePipeline, LLGL::RenderTarget* writeTarget,
                                   LLGL::Texture* writeableTexture) const
{
	// Set the render target and clear the color buffer
	commandBuffer.BeginRenderPass(*writeTarget);
	{
		// Clear color and depth buffers of active framebuffer (i.e. the render target)
		commandBuffer.Clear(LLGL::ClearFlags::ColorDepth, {LLGL::ColorRGBAf{0.2f, 0.7f, 0.1f}});

		// Bind graphics pipeline for render target
		commandBuffer.SetPipelineState(*writePipeline);
		commandBuffer.SetVertexBuffer(*mVertexBuffer);

		const auto textureId = ResourceManager::GetTextureId(map->GetTexturePath());
		commandBuffer.SetResourceHeap(*mResourceHeap, textureId);

		const auto mapData = map->GetData();
		for (int i = 0; i < mapData.size(); i++)
		{
			glm::vec3 tilePos;
			glm::vec3 size;
			glm::vec4 clip;
			map->CalculateTileDrawData(i, tilePos, size, clip);

			_UpdateUniformsModel(commandBuffer, tilePos, size, map->GetRotation(), clip);
			commandBuffer.Draw(4, 0);
		}
	}
	commandBuffer.EndRenderPass();

	commandBuffer.GenerateMips(*writeableTexture);
}
