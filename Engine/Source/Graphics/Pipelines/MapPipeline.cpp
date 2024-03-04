#include "LLGL/Utils/Parse.h"
#include "LLGL/Utils/VertexFormat.h"
#include "LLGL/Utils/Utility.h"

#include "Core/Camera.h"
#include "Entity/EntityRegistry.h"
#include "Entity/Components/MapComponent.h"
#include "Entity/Components/TransformComponent.h"
#include "Graphics/Core/ResourceManager.h"
#include "Graphics/Core/Texture.h"

#include "MapPipeline.h"

void MapPipeline::Render(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat,
                         EntityRegistry& entityRegistry) const
{
	// set graphics pipeline
	commandBuffer.SetPipelineState(*mPipeline);
	commandBuffer.SetVertexBuffer(*mVertexBuffer);

	const auto mapView = entityRegistry.GetEnttRegistry().view<const TransformComponent, const MapComponent>();
	mapView.each([this, &commandBuffer, &pvMat](const TransformComponent& transform, const MapComponent& map)
	{
		commandBuffer.SetResourceHeap(*mMapResourceHeap, map.mGeneratedTextureId);
		_UpdateUniforms(commandBuffer, pvMat, transform);
		commandBuffer.Draw(4, 0);
	});
}

void MapPipeline::_UpdateUniforms(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat,
                                  const TransformComponent& transform) const
{
	// Update
	auto model = glm::mat4(1.0f);
	model = translate(model, transform.mPosition);
	model = translate(model, glm::vec3(0.5f * transform.mSize.x, 0.5f * transform.mSize.y, 0.0f));
	// Apply rotation in ZXY order
	model = rotate(model, glm::radians(transform.mRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	model = rotate(model, glm::radians(transform.mRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = rotate(model, glm::radians(transform.mRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));

	model = translate(model, glm::vec3(-0.5f * transform.mSize.x, -0.5f * transform.mSize.y, 0.0f));
	model = scale(model, transform.mSize);

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
	mSampler = renderSystem->CreateSampler(samplerDesc);

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

void MapPipeline::_CreateMapResourceHeap(const LLGL::RenderSystemPtr& renderSystem)
{
	std::vector<LLGL::ResourceViewDescriptor> resourceViews;
	resourceViews.reserve(mGeneratedTextures.size() * 3);

	for (const auto& texture : mGeneratedTextures)
	{
		resourceViews.emplace_back(mConstantBuffer);
		resourceViews.emplace_back(texture);
		resourceViews.emplace_back(mSampler);
	}

	// Add to Map ResourceHeap
	mMapResourceHeap = renderSystem->CreateResourceHeap(mPipelineLayout, resourceViews);
}

void MapPipeline::GenerateMapTexture(const LLGL::RenderSystemPtr& renderSystem,
                                     LLGL::CommandBuffer& commandBuffer, EntityRegistry& entityRegistry,
                                     EntityId mapId)
{
	_InitMapTexturePipeline(commandBuffer, renderSystem, entityRegistry, mapId);
}

void MapPipeline::_InitMapTexturePipeline(LLGL::CommandBuffer& commandBuffer,
                                          const LLGL::RenderSystemPtr& renderSystem,
                                          EntityRegistry& entityRegistry,
                                          EntityId mapId)
{
	// Create Render Target
	const auto texture = renderSystem->CreateTexture(
		LLGL::Texture2DDesc(LLGL::Format::RGBA8UNorm, mTextureSize.width, mTextureSize.height)
	);

	// Create render-target with multi-sampling
	LLGL::RenderTargetDescriptor renderTargetDesc;
	{
		renderTargetDesc.resolution = mTextureSize;
		renderTargetDesc.colorAttachments[0] = texture;
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
	_WriteMapTexture(commandBuffer, pipeline, renderTarget, texture, entityRegistry, mapId);

	_CreateMapResourceHeap(renderSystem);
}

void MapPipeline::_WriteMapTexture(LLGL::CommandBuffer& commandBuffer, LLGL::PipelineState* writePipeline,
                                   LLGL::RenderTarget* writeTarget, LLGL::Texture* writtenTexture,
                                   EntityRegistry& entityRegistry, EntityId mapId)
{
	auto mapComponent = entityRegistry.GetComponent<MapComponent>(mapId);
	auto transformComponent = entityRegistry.GetComponent<TransformComponent>(mapId);
	// Set the render target and clear the color buffer
	commandBuffer.BeginRenderPass(*writeTarget);
	{
		// Clear color and depth buffers of active framebuffer (i.e. the render target)
		// TODO: {LLGL::ColorRGBAf{0.2f, 0.7f, 0.1f}} ? Clear with this color when drawing map?
		commandBuffer.Clear(LLGL::ClearFlags::ColorDepth);

		// Bind graphics pipeline for render target
		commandBuffer.SetPipelineState(*writePipeline);
		commandBuffer.SetVertexBuffer(*mVertexBuffer);

		const auto textureId = ResourceManager::GetTextureId(mapComponent.mTexturePath);
		commandBuffer.SetResourceHeap(*mResourceHeap, textureId);

		const auto& mapData = mapComponent.mData;
		for (int i = 0; i < mapData.size(); i++)
		{
			glm::vec3 tilePos;
			glm::vec3 size;
			glm::vec4 clip;
			_CalculateTileDrawData(mapComponent, i, tilePos, size, clip);

			_UpdateUniformsModel(commandBuffer, tilePos, size, transformComponent.mRotation, clip);
			commandBuffer.Draw(4, 0);
		}
	}
	commandBuffer.EndRenderPass();

	commandBuffer.GenerateMips(*writtenTexture);

	// Does this ptr need to be moved or cleaned up in any way?
	int newTextureId = static_cast<int>(mGeneratedTextures.size());
	mGeneratedTextures.push_back(writtenTexture);

	mapComponent.mGeneratedTextureId = newTextureId;
}

void MapPipeline::_CalculateTileDrawData(const MapComponent& mapComponent, int tileIndex, glm::vec3& pos,
                                         glm::vec3& size, glm::vec4& clip) const
{
	const auto rows = mapComponent.mRows;
	const auto columns = mapComponent.mColumns;
	const glm::vec2 tileSize = {(1.0f / columns) * 2, (1.0f / rows) * 2};
	const glm::vec3 mapTopLeft = {-1 + (tileSize.x / 2.0f), 1 - (tileSize.y / 2.0f), 0};

	const int tile = mapComponent.mData[tileIndex];
	const float posX = tileIndex % columns;
	const float currentRow = floorf(tileIndex / columns);

	clip = _GetClipForTile(mapComponent, tile);

	pos = {
		mapTopLeft.x + (posX * tileSize.x),
		mapTopLeft.y - (currentRow * tileSize.y),
		mapTopLeft.z
	};
	size = {tileSize.x, tileSize.y, 0};
}

glm::vec4 MapPipeline::_GetClipForTile(const MapComponent& mapComponent, int index) const
{
	const auto texColumns = mapComponent.mTextureMapColumns;
	const auto texRows = mapComponent.mTextureMapRows;
	const auto whichColumn = fmod(index, texColumns);
	// Add one or else the last index in row tries to go to next row
	const auto whichRow = floor(index / (texRows + 1));
	const glm::vec2 clipStart = {
		whichColumn / texColumns, whichRow / texRows
	};
	return {
		clipStart.x, clipStart.y, 1.0 / texColumns, 1.0 / texRows
	};
}
