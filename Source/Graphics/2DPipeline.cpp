#include "LLGL/Misc/VertexFormat.h"
#include "LLGL/Misc/Utility.h"

#include "Core/Renderer.h"
#include "Core/ResourceManager.h"
#include "Components/MapComponent.h"
#include "Components/TransformComponent.h"
#include "Components/SpriteComponent.h"
#include "Camera.h"
#include "LevelManager.h"
#include "Map.h"
#include "MapDescription.h"
#include "MapSystem.h"
#include "EntityRegistry.h"
#include "Texture.h"

#include "2DPipeline.h"

Pipeline2D::Pipeline2D(EntityRegistry& entityRegistry, LevelManager& levelManager, MapSystem& mapSystem,
                       Renderer& renderer, ResourceManager& resourceManager)
	: mEntityRegistry(entityRegistry), mLevelManager(levelManager), mMapSystem(mapSystem), mRenderer(renderer),
	  mResourceManager(resourceManager)
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

void Pipeline2D::Tick() const
{
	auto& commands = mRenderer.GetCommandBuffer();

	// set graphics pipeline
	commands.SetPipelineState(*mPipeline);
	commands.SetVertexBuffer(*mVertexBuffer);

	const auto spriteView = mEntityRegistry.GetEnttRegistry().view<const TransformComponent, const SpriteComponent>(
		entt::exclude<MapComponent>);
	spriteView.each([=](const TransformComponent& transform, const SpriteComponent& sprite)
	{
		Render(transform, sprite);
	});

	// Render map
	for (const auto& map : mMapSystem.GetAllMaps())
	{
		if (map->GetRenderDebug())
		{
			RenderMapTexture(map);
		}
		else
		{
			RenderMap(map);
		}
	}
}

void Pipeline2D::Render(const TransformComponent& transform, const SpriteComponent& sprite) const
{
	auto& commands = mRenderer.GetCommandBuffer();
	const auto textureId = mResourceManager.GetTextureId(sprite.mTexturePath);

	// If this assert is hit, need to add a case for commands.SetResourceHeap(*mResourceHeap);
	assert(textureId != -1);

	commands.SetResourceHeap(*mResourceHeap, textureId);

	_UpdateUniforms(transform);

	commands.Draw(4, 0);
}

void Pipeline2D::RenderMap(const std::shared_ptr<Map>& map) const
{
	auto& commands = mRenderer.GetCommandBuffer();
	commands.SetResourceHeap(*mResourceHeap, map->GetMapTextureId());
	_UpdateUniforms(map->GetPosition(), map->GetMapSize(),
	                map->GetRotation());
	commands.Draw(4, 0);
}

void Pipeline2D::RenderMapTexture(const std::shared_ptr<Map>& map) const
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

void Pipeline2D::_UpdateUniforms(const TransformComponent& transform) const
{
	auto& commands = mRenderer.GetCommandBuffer();
	// Update
	auto model = glm::mat4(1.0f);
	model = translate(model, transform.mPosition);
	model = translate(model, glm::vec3(0.5f * transform.mSize.x, 0.5f * transform.mSize.y, 0.0f));
	model = rotate(model, glm::radians(transform.mRotation), glm::vec3(0.0f, 0.0f, 1.0f));
	model = translate(model, glm::vec3(-0.5f * transform.mSize.x, -0.5f * transform.mSize.y, 0.0f));
	model = scale(model, transform.mSize);

	const auto textureClip = glm::mat4(1.0f);

	const auto& camera = mLevelManager.GetCurrentLevel()->GetCamera();

	const SpriteSettings settings = {mRenderer.GetProjection() * camera->GetView() * model, textureClip};
	commands.UpdateBuffer(*mConstantBuffer, 0, &settings, sizeof(settings));
}

void Pipeline2D::_UpdateUniforms(glm::vec3 pos, glm::vec3 size, glm::vec3 rot) const
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

	const auto& camera = mLevelManager.GetCurrentLevel()->GetCamera();

	const SpriteSettings settings = {mRenderer.GetProjection() * camera->GetView() * model, textureClip};
	commands.UpdateBuffer(*mConstantBuffer, 0, &settings, sizeof(settings));
}

void Pipeline2D::_UpdateUniformsModel(glm::vec3 pos, glm::vec3 size, glm::vec3 rot, glm::vec4 texClip) const
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

void Pipeline2D::_InitPipeline()
{
	mConstantBuffer = mRenderer.GetRendererSystem()->CreateBuffer(LLGL::ConstantBufferDesc(sizeof(SpriteSettings)),
	                                                              &spriteSettings);

	LLGL::VertexFormat vertexFormat;
	vertexFormat.AppendAttribute({"position", LLGL::Format::RGB32Float});
	vertexFormat.AppendAttribute({"color", LLGL::Format::RGB32Float});
	vertexFormat.AppendAttribute({"texCoord", LLGL::Format::RG32Float});

	mShader = std::make_unique<Shader>(*mRenderer.GetRendererSystem(), vertexFormat, "sprite.vert", "sprite.frag");

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

void Pipeline2D::_CreateResourceHeap()
{
	// Resource Heap
	const auto& textures = mResourceManager.getTextures();

	const auto& maps = mMapSystem.GetAllMaps();
	const int numMaps = static_cast<int>(maps.size());

	LLGL::ResourceHeapDescriptor resourceHeapDesc;
	{
		resourceHeapDesc.pipelineLayout = mPipelineLayout;
		resourceHeapDesc.resourceViews.reserve((textures.size() + numMaps) * 3);

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
			resourceHeapDesc.resourceViews.emplace_back(&textures.at(0)->GetSamplerData());
			i++;
		}
	}
	mResourceHeap = mRenderer.GetRendererSystem()->CreateResourceHeap(resourceHeapDesc);
}

void Pipeline2D::QueueWriteMapTexture(const std::shared_ptr<Map>& map)
{
	mQueuedMaps.emplace_back(map);
}

void Pipeline2D::WriteQueuedMapTextures()
{
	if (mQueuedMaps.empty()) return;

	for (auto& map : mQueuedMaps)
	{
		_InitMapTexturePipeline(map);
		_WriteMapTexture(map);
	}

	mQueuedMaps.clear();
}

void Pipeline2D::_InitMapTexturePipeline(std::shared_ptr<Map>& map)
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

void Pipeline2D::_WriteMapTexture(std::shared_ptr<Map>& map) const
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
