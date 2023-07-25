#include "LLGL/Misc/VertexFormat.h"
#include "LLGL/Misc/Utility.h"

#include "Core/Camera.h"
#include "Core/LevelManager.h"
#include "Core/Renderer.h"
#include "Core/ResourceManager.h"
#include "Entity/EntityRegistry.h"
#include "Map/Map.h"
#include "Map/MapDescription.h"
#include "Map/MapSystem.h"
#include "Texture.h"

#include "MapPipeline.h"

MapPipeline::MapPipeline(LevelManager& levelManager, MapSystem& mapSystem, Renderer& renderer,
                         ResourceManager& resourceManager, std::string shadersFolderPath)
	: mLevelManager(levelManager), mMapSystem(mapSystem), mRenderer(renderer),
	  mResourceManager(resourceManager), mShadersFolderPath(std::move(shadersFolderPath))
{
	_InitPipeline();

	mVertexBuffer = mRenderer.GetRendererSystem()->CreateBuffer(
		VertexBufferDesc(static_cast<std::uint32_t>(mVertices.size() * sizeof(Vertex)),
		                 mShader->GetVertexFormat()),
		mVertices.data()
	);

	mMapSystem.RegisterOnCreateCallback([=](const std::shared_ptr<Map>& map) { QueueWriteMapTexture(map); });

	// Callback only usable when editor is active, not for in-game use
	mMapSystem.RegisterOnUpdateCallback([=]() { QueueWriteMapTexture(mMapSystem.GetCurrentMap()); });
}

void MapPipeline::Tick() const
{
	auto& commands = mRenderer.GetCommandBuffer();

	// set graphics pipeline
	commands.SetPipelineState(*mPipeline);
	commands.SetVertexBuffer(*mVertexBuffer);

	// Render map, draw first so other sprites are drawn on top
	for (const auto& map : mMapSystem.GetAllMaps())
	{
		if (map->GetRenderDebug())
		{
			RenderMapTexture(map);
		}
		else
		{
			Render(map);
		}
	}
}

void MapPipeline::Render(const std::shared_ptr<Map>& map) const
{
	auto& commands = mRenderer.GetCommandBuffer();
	commands.SetResourceHeap(*mResourceHeap, map->GetMapTextureId());
	_UpdateUniforms(map->GetPosition(), map->GetMapSize(),
	                map->GetRotation());
	commands.Draw(4, 0);
}

void MapPipeline::RenderMapTexture(const std::shared_ptr<Map>& map) const
{
	auto& commands = mRenderer.GetCommandBuffer();
	const auto textureId = mResourceManager.GetTextureId(map->GetMapDescription()->GetTexturePath());

	// If this assert is hit, need to add a case for commands.SetResourceHeap(*mResourceHeap);
	assert(textureId != -1);

	commands.SetResourceHeap(*mResourceHeap, textureId);
	const auto pos = map->GetPosition();
	const auto mapSize = map->GetMapSize();
	_UpdateUniforms(pos, mapSize, map->GetRotation());
	commands.Draw(4, 0);
}

void MapPipeline::_UpdateUniforms(glm::vec3 pos, glm::vec3 size, glm::vec3 rot) const
{
	auto& commands = mRenderer.GetCommandBuffer();
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

	if (const auto& level = mLevelManager.GetCurrentLevel())
	{
		const auto& camera = level->GetCamera();
		const SpriteSettings settings = {mRenderer.GetProjection() * camera->GetView() * model, textureClip};
		commands.UpdateBuffer(*mConstantBuffer, 0, &settings, sizeof(settings));
	}
}

void MapPipeline::_UpdateUniformsModel(glm::vec3 pos, glm::vec3 size, glm::vec3 rot, glm::vec4 texClip) const
{
	auto& commands = mRenderer.GetCommandBuffer();
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
	commands.UpdateBuffer(*mConstantBuffer, 0, &settings, sizeof(settings));
}

void MapPipeline::_InitPipeline()
{
	mConstantBuffer = mRenderer.GetRendererSystem()->CreateBuffer(LLGL::ConstantBufferDesc(sizeof(SpriteSettings)),
	                                                              &spriteSettings);

	LLGL::VertexFormat vertexFormat;
	vertexFormat.AppendAttribute({"position", LLGL::Format::RGB32Float});
	vertexFormat.AppendAttribute({"color", LLGL::Format::RGB32Float});
	vertexFormat.AppendAttribute({"texCoord", LLGL::Format::RG32Float});

	std::string vertPath = mShadersFolderPath;
	vertPath.append("sprite.vert");

	std::string fragPath = mShadersFolderPath;
	fragPath.append("sprite.frag");

	mShader = std::make_unique<Shader>(*mRenderer.GetRendererSystem(), vertexFormat, vertPath.c_str(),
	                                   fragPath.c_str());

	// All layout bindings that will be used by graphics and compute pipelines
	// Create pipeline layout
	mPipelineLayout = mRenderer.GetRendererSystem()->CreatePipelineLayout(LLGL::PipelineLayoutDesc(
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
	mPipeline = mRenderer.GetRendererSystem()->CreatePipelineState(pipelineDesc);

	_CreateResourceHeap();
}

void MapPipeline::_CreateResourceHeap()
{
	// Resource Heap
	const auto& maps = mMapSystem.GetAllMaps();
	if (maps.empty())
	{
		return;
	}

	const auto& textures = mResourceManager.getTextures();

	// 1st sampler state with default settings
	// Create nearest sampler
	LLGL::SamplerDescriptor samplerDesc;
	{
		samplerDesc.minFilter = LLGL::SamplerFilter::Nearest;
		samplerDesc.magFilter = LLGL::SamplerFilter::Nearest;
		samplerDesc.mipMapFilter = LLGL::SamplerFilter::Nearest;
	}
	const auto sampler = mRenderer.GetRendererSystem()->CreateSampler(samplerDesc);

	LLGL::ResourceHeapDescriptor resourceHeapDesc;
	{
		resourceHeapDesc.pipelineLayout = mPipelineLayout;
		resourceHeapDesc.resourceViews.reserve((textures.size() + maps.size()) * 3);

		for (const auto& texture : textures)
		{
			resourceHeapDesc.resourceViews.emplace_back(mConstantBuffer);
			resourceHeapDesc.resourceViews.emplace_back(&texture.second->GetTextureData());
			resourceHeapDesc.resourceViews.emplace_back(&texture.second->GetSamplerData());
		}

		int i = static_cast<int>(textures.size());
		for (auto& map : maps)
		{
			map->SetMapTextureId(i);
			resourceHeapDesc.resourceViews.emplace_back(mConstantBuffer);
			resourceHeapDesc.resourceViews.emplace_back(&map->GetMapTexture());
			resourceHeapDesc.resourceViews.emplace_back(sampler);
			i++;
		}
	}
	mResourceHeap = mRenderer.GetRendererSystem()->CreateResourceHeap(resourceHeapDesc);
}

void MapPipeline::QueueWriteMapTexture(const std::shared_ptr<Map>& map)
{
	mQueuedMaps.emplace_back(map);
}

void MapPipeline::WriteQueuedMapTextures()
{
	if (mQueuedMaps.empty()) return;

	for (auto& map : mQueuedMaps)
	{
		_InitMapTexturePipeline(map);
		_WriteMapTexture(map);
	}

	mQueuedMaps.clear();
}

void MapPipeline::_InitMapTexturePipeline(std::shared_ptr<Map>& map)
{
	const auto& renderTargetSize = map->GetMapTextureSize();

	// Create Render Target
	const auto renderTargetTex = mRenderer.GetRendererSystem()->CreateTexture(
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
	const auto renderTarget = mRenderer.GetRendererSystem()->CreateRenderTarget(renderTargetDesc);
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

	const auto pipeline = mRenderer.GetRendererSystem()->CreatePipelineState(pipelineDesc);
	map->SetMapTexturePipeline(pipeline);
	_CreateResourceHeap();
}

void MapPipeline::_WriteMapTexture(const std::shared_ptr<Map>& map) const
{
	auto& commands = mRenderer.GetCommandBuffer();

	// Set the render target and clear the color buffer
	commands.BeginRenderPass(map->GetMapRenderTarget());
	{
		// Clear color and depth buffers of active framebuffer (i.e. the render target)
		commands.Clear(LLGL::ClearFlags::ColorDepth, {LLGL::ColorRGBAf{0.2f, 0.7f, 0.1f}});

		// Bind graphics pipeline for render target
		commands.SetPipelineState(map->GetMapTexturePipeline());
		commands.SetVertexBuffer(*mVertexBuffer);

		const auto textureId = mResourceManager.GetTextureId(map->GetMapDescription()->GetTexturePath());
		commands.SetResourceHeap(*mResourceHeap, textureId);
		//commands.SetResource(map->GetMapTexture(), 1, LLGL::BindFlags::Sampled, mMapShaderStages);

		const auto mapData = map->GetMapDescription()->GetMapData();
		for (int i = 0; i < mapData.size(); i++)
		{
			glm::vec3 tilePos;
			glm::vec3 size;
			glm::vec4 clip;
			map->CalculateTileDrawData(i, tilePos, size, clip);

			_UpdateUniformsModel(tilePos, size, map->GetRotation(), clip);
			commands.Draw(4, 0);
		}
	}
	commands.EndRenderPass();

	commands.GenerateMips(map->GetMapTexture());
}
