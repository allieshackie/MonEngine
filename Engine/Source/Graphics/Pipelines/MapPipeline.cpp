#include "LLGL/Utils/Parse.h"
#include "LLGL/Utils/VertexFormat.h"
#include "LLGL/Utils/Utility.h"

#include "MeshPipeline.h"
#include "Core/Camera.h"
#include "Entity/EntityRegistry.h"
#include "Entity/Components/MapComponent.h"
#include "Entity/Components/MeshComponent.h"
#include "Entity/Components/TransformComponent.h"
#include "Graphics/Core/ResourceManager.h"
#include "Graphics/Core/Texture.h"

#include "MapPipeline.h"

MapPipeline::MapPipeline(const LLGL::RenderSystemPtr& renderSystem, const ResourceManager& resourceManager,
                         LLGL::Buffer* meshConstantBuffer)
	: PipelineBase(),
	  mMeshConstantBuffer(meshConstantBuffer)
{
	// Initialization
	{
		InitConstantBuffer<Settings>(renderSystem, settings);

		LLGL::VertexFormat vertexFormat;
		vertexFormat.AppendAttribute({"position", LLGL::Format::RGB32Float, 0, 0, sizeof(Vertex), 0});
		vertexFormat.AppendAttribute({"normal", LLGL::Format::RGB32Float, 1, 12, sizeof(Vertex), 0});
		vertexFormat.AppendAttribute({"texCoord", LLGL::Format::RG32Float, 2, 24, sizeof(Vertex), 0});
		InitShader(renderSystem, vertexFormat, "sprite.vert", "sprite.frag");
		LLGL::PipelineLayoutDescriptor layoutDesc;
		{
			layoutDesc.heapBindings =
			{
				LLGL::BindingDescriptor{
					"Settings", LLGL::ResourceType::Buffer, LLGL::BindFlags::ConstantBuffer,
					LLGL::StageFlags::VertexStage, 0
				},
				LLGL::BindingDescriptor{
					"colorMap", LLGL::ResourceType::Texture, LLGL::BindFlags::Sampled, LLGL::StageFlags::FragmentStage,
					3
				},
				LLGL::BindingDescriptor{
					LLGL::ResourceType::Sampler, 0, LLGL::StageFlags::FragmentStage, 3
				},

			};
		}
		InitPipeline(renderSystem, layoutDesc);

		// Resource Heap
		const auto& textures = resourceManager.GetTextures();

		std::vector<LLGL::ResourceViewDescriptor> resourceViews;
		resourceViews.reserve(textures.size() * 3);

		for (const auto& texture : textures)
		{
			resourceViews.emplace_back(mConstantBuffer);
			resourceViews.emplace_back(&texture->GetTextureData());
			resourceViews.emplace_back(&texture->GetSamplerData());
		}
		InitResourceHeap(renderSystem, resourceViews);
	}

	// Create nearest sampler
	LLGL::SamplerDescriptor samplerDesc;
	{
		samplerDesc.minFilter = LLGL::SamplerFilter::Nearest;
		samplerDesc.magFilter = LLGL::SamplerFilter::Nearest;
		samplerDesc.mipMapFilter = LLGL::SamplerFilter::Nearest;
	}
	mSampler = renderSystem->CreateSampler(samplerDesc);

	mVertexBuffer = renderSystem->CreateBuffer(
		VertexBufferDesc(static_cast<std::uint32_t>(mVertices.size() * sizeof(Vertex)),
		                 mShader->GetVertexFormat()), mVertices.data()
	);
}

void MapPipeline::Render(LLGL::CommandBuffer& commandBuffer, const Camera& camera,
                         const glm::mat4 projection, EntityRegistry& entityRegistry,
                         const LLGL::RenderSystemPtr& renderSystem,
                         ResourceManager& resourceManager,
                         MeshPipeline& meshPipeline) const
{
	commandBuffer.SetPipelineState(*mPipeline);

	const auto map3DView = entityRegistry.GetEnttRegistry().view<
		const MapComponent, const TransformComponent, MeshComponent>();

	map3DView.each([this, &commandBuffer, &camera, &renderSystem, &resourceManager, &projection, &meshPipeline](
		const MapComponent& map,
		const TransformComponent& transform,
		MeshComponent& mesh)
		{
			// Set resources
			meshPipeline.SetPipeline(commandBuffer);
			if (map.mGeneratedTextureId != -1)
			{
				commandBuffer.SetResourceHeap(*mMapMeshResourceHeap, map.mGeneratedTextureId);
			}
			else
			{
				auto& texture = resourceManager.GetTexture(map.mTexturePath);
				meshPipeline.SetResourceHeapTexture(commandBuffer, texture);
			}
			meshPipeline.RenderMap(commandBuffer, camera, renderSystem, resourceManager, projection, mesh, transform);
		});
}

void MapPipeline::_UpdateUniformsModel(LLGL::CommandBuffer& commandBuffer, glm::vec3 pos, glm::vec3 size, glm::vec3 rot,
                                       glm::vec4 texClip)
{
	// Update
	auto model = glm::mat4(1.0f);
	model = translate(model, pos);
	// Apply rotation in ZXY order
	model = rotate(model, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
	model = rotate(model, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = rotate(model, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));

	model = scale(model, size);

	auto textureClip = glm::mat4(1.0f);
	textureClip = translate(textureClip, glm::vec3(texClip.x, texClip.y, 0.0f));
	textureClip = scale(textureClip, glm::vec3(texClip.z, texClip.w, 1.0f));

	settings.model = model;
	settings.view = glm::mat4(1.0f);
	settings.projection = glm::mat4(1.0f);
	settings.textureClip = textureClip;

	commandBuffer.UpdateBuffer(*mConstantBuffer, 0, &settings, sizeof(settings));
}

void MapPipeline::_CreateMapResourceHeap(const LLGL::RenderSystemPtr& renderSystem)
{
	// Release previous resource heap if set
	renderSystem->Release(*mMapMeshResourceHeap);

	std::vector<LLGL::ResourceViewDescriptor> meshResourceViews;
	meshResourceViews.reserve(mGeneratedTextures.size() * 3);

	for (const auto& texture : mGeneratedTextures)
	{
		meshResourceViews.emplace_back(mMeshConstantBuffer);
		meshResourceViews.emplace_back(texture);
		meshResourceViews.emplace_back(mSampler);
	}

	// Add to Map ResourceHeap
	mMapMeshResourceHeap = renderSystem->CreateResourceHeap(mPipelineLayout, meshResourceViews);
}

void MapPipeline::GenerateMapTexture(const LLGL::RenderSystemPtr& renderSystem,
                                     LLGL::CommandBuffer& commandBuffer, EntityRegistry& entityRegistry,
                                     const ResourceManager& resourceManager, EntityId mapId)
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
	_WriteMapTexture(commandBuffer, pipeline, renderTarget, texture, entityRegistry, resourceManager, mapId);

	_CreateMapResourceHeap(renderSystem);
}

void MapPipeline::_WriteMapTexture(LLGL::CommandBuffer& commandBuffer, LLGL::PipelineState* writePipeline,
                                   LLGL::RenderTarget* writeTarget, LLGL::Texture* writtenTexture,
                                   EntityRegistry& entityRegistry, const ResourceManager& resourceManager,
                                   EntityId mapId)
{
	auto& mapComponent = entityRegistry.GetComponent<MapComponent>(mapId);
	const auto& transformComponent = entityRegistry.GetComponent<TransformComponent>(mapId);
	// Set the render target and clear the color buffer
	commandBuffer.BeginRenderPass(*writeTarget);
	{
		// Clear color and depth buffers of active framebuffer (i.e. the render target)
		// TODO: {LLGL::ColorRGBAf{0.2f, 0.7f, 0.1f}} ? Clear with this color when drawing map?
		commandBuffer.Clear(LLGL::ClearFlags::ColorDepth);

		// Bind graphics pipeline for render target
		commandBuffer.SetPipelineState(*writePipeline);
		commandBuffer.SetVertexBuffer(*mVertexBuffer);

		const auto textureId = resourceManager.GetTextureId(mapComponent.mTexturePath);
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
