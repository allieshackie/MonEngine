#include "LLGL/Misc/VertexFormat.h"
#include "LLGL/Misc/Utility.h"

#include "Core/Camera.h"
#include "Core/ResourceManager.h"
#include "Entity/EntityRegistry.h"
#include "Map/Map.h"
#include "Map/MapDescription.h"
#include "Texture.h"

#include "MapPipeline.h"

void MapPipeline::Render(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat,
                         const std::shared_ptr<Map>& map) const
{
	// set graphics pipeline
	commandBuffer.SetPipelineState(*mPipeline);
	commandBuffer.SetVertexBuffer(*mVertexBuffer);

	// TODO: Should we be rendering more than 1 map at a time?  Sublevels/chunks?
	if (map->GetRenderDebug())
	{
		RenderMapTexture(commandBuffer, pvMat, map);
	}
	else
	{
		_Render(commandBuffer, pvMat, map);
	}
}

void MapPipeline::_Render(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat,
                          const std::shared_ptr<Map>& map) const
{
	// TODO: Is this the correct texture ID? (Map Texture vs the texture used to create the map)
	//commandBuffer.SetResourceHeap(*mResourceHeap, map->GetMapTextureId());
	_UpdateUniforms(commandBuffer, pvMat, map->GetPosition(), map->GetMapSize(),
	                map->GetRotation());
	commandBuffer.Draw(4, 0);
}

void MapPipeline::RenderMapTexture(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat,
                                   const std::shared_ptr<Map>& map) const
{
	// TODO: Is this the correct texture ID? (Map Texture vs the texture used to create the map)
	//commandBuffer.SetResourceHeap(*mResourceHeap, map->GetMapTextureId());
	const auto pos = map->GetPosition();
	const auto mapSize = map->GetMapSize();
	_UpdateUniforms(commandBuffer, pvMat, pos, mapSize, map->GetRotation());
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

void MapPipeline::Init(std::unique_ptr<LLGL::RenderSystem>& renderSystem, const std::string& shaderPath,
                       const TextureMap& textures)
{
	mConstantBuffer = renderSystem->CreateBuffer(LLGL::ConstantBufferDesc(sizeof(SpriteSettings)),
	                                             &spriteSettings);

	LLGL::VertexFormat vertexFormat;
	vertexFormat.AppendAttribute({"position", LLGL::Format::RGB32Float});
	vertexFormat.AppendAttribute({"color", LLGL::Format::RGB32Float});
	vertexFormat.AppendAttribute({"texCoord", LLGL::Format::RG32Float});

	std::string vertPath = shaderPath;
	vertPath.append("sprite.vert");

	std::string fragPath = shaderPath;
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

	// TODO: How can this work? 
	//_CreateResourceHeap(renderSystem, textures);

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

void MapPipeline::_CreateResourceHeap(const std::unique_ptr<LLGL::RenderSystem>& renderSystem,
                                      const TextureMap& textures, const std::shared_ptr<Map>& map)
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

	LLGL::ResourceHeapDescriptor resourceHeapDesc;
	{
		resourceHeapDesc.pipelineLayout = mPipelineLayout;
		resourceHeapDesc.resourceViews.reserve((textures.size() + 1) * 3);

		for (const auto& texture : textures)
		{
			resourceHeapDesc.resourceViews.emplace_back(mConstantBuffer);
			resourceHeapDesc.resourceViews.emplace_back(&texture.second->GetTextureData());
			resourceHeapDesc.resourceViews.emplace_back(&texture.second->GetSamplerData());
		}


		// TODO: This used to handle multiple maps, what should we do now?
		const int i = static_cast<int>(textures.size());

		map->SetMapTextureId(i);
		resourceHeapDesc.resourceViews.emplace_back(mConstantBuffer);
		resourceHeapDesc.resourceViews.emplace_back(&map->GetMapTexture());
		resourceHeapDesc.resourceViews.emplace_back(sampler);
	}
	mResourceHeap = renderSystem->CreateResourceHeap(resourceHeapDesc);
}

void MapPipeline::QueueWriteMapTexture(const std::shared_ptr<Map>& map)
{
	mQueuedMaps.emplace_back(map);
}

void MapPipeline::WriteQueuedMapTextures(const std::unique_ptr<LLGL::RenderSystem>& renderSystem,
                                         LLGL::CommandBuffer& commandBuffer,
                                         const TextureMap& textures)
{
	if (mQueuedMaps.empty()) return;

	for (auto& map : mQueuedMaps)
	{
		_InitMapTexturePipeline(renderSystem, textures, map);
		_WriteMapTexture(commandBuffer, map);
	}

	mQueuedMaps.clear();
}

void MapPipeline::_InitMapTexturePipeline(const std::unique_ptr<LLGL::RenderSystem>& renderSystem,
                                          const TextureMap& textures, std::shared_ptr<Map>& map)
{
	const auto& renderTargetSize = map->GetMapTextureSize();

	// Create Render Target
	const auto renderTargetTex = renderSystem->CreateTexture(
		LLGL::Texture2DDesc(LLGL::Format::RGBA8UNorm, renderTargetSize.width, renderTargetSize.height)
	);
	map->SetMapTexture(renderTargetTex);

	// Create render-target with multi-sampling
	LLGL::RenderTargetDescriptor renderTargetDesc;
	{
		renderTargetDesc.resolution = renderTargetSize;

		renderTargetDesc.attachments =
		{
			LLGL::AttachmentDescriptor{LLGL::AttachmentType::Depth},
			LLGL::AttachmentDescriptor{LLGL::AttachmentType::Color, renderTargetTex}
		};
	}
	const auto renderTarget = renderSystem->CreateRenderTarget(renderTargetDesc);
	map->SetMapRenderTarget(renderTarget);

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
	map->SetMapTexturePipeline(pipeline);
	_CreateResourceHeap(renderSystem, textures, map);
}

void MapPipeline::_WriteMapTexture(LLGL::CommandBuffer& commandBuffer, const std::shared_ptr<Map>& map) const
{
	// Set the render target and clear the color buffer
	commandBuffer.BeginRenderPass(map->GetMapRenderTarget());
	{
		// Clear color and depth buffers of active framebuffer (i.e. the render target)
		commandBuffer.Clear(LLGL::ClearFlags::ColorDepth, {LLGL::ColorRGBAf{0.2f, 0.7f, 0.1f}});

		// Bind graphics pipeline for render target
		commandBuffer.SetPipelineState(map->GetMapTexturePipeline());
		commandBuffer.SetVertexBuffer(*mVertexBuffer);

		// TODO: Is this the right textureId? 
		//const auto textureId = mResourceManager.GetTextureId(map->GetMapDescription()->GetTexturePath());
		commandBuffer.SetResourceHeap(*mResourceHeap, map->GetMapTextureId());

		const auto mapData = map->GetMapDescription()->GetMapData();
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

	commandBuffer.GenerateMips(map->GetMapTexture());
}
