#include "LLGL/Misc/VertexFormat.h"
#include "LLGL/Misc/Utility.h"

#include "Core/Renderer.h"
#include "Core/ResourceManager.h"
#include "Core/Shader.h"
#include "Components/MapComponent.h"
#include "Components/TransformComponent.h"
#include "Components/SpriteComponent.h"
#include "EntityRegistry.h"
#include "Texture.h"

#include "2DPipeline.h"

Pipeline2D::Pipeline2D(Renderer& renderer, ResourceManager& resourceManager, EntityRegistry& entityRegistry)
	: mRenderer(renderer), mResourceManager(resourceManager), mEntityRegistry(entityRegistry)
{
	_InitPipeline();

	mVertexBuffer = mRenderer.GetRendererSystem()->CreateBuffer(
		VertexBufferDesc(static_cast<std::uint32_t>(mVertices.size() * sizeof(Vertex)),
		                 mShader->GetVertexFormat()),
		mVertices.data()
	);
}

Pipeline2D::~Pipeline2D()
{
	delete mPipeline;
	delete mConstantBuffer;
	delete mVertexBuffer;
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

	const auto mapView = mEntityRegistry.GetEnttRegistry().view<
		const TransformComponent, const SpriteComponent, const MapComponent>();
	mapView.each([=](const TransformComponent& transform, const SpriteComponent& sprite, const MapComponent& map)
	{
		RenderMap(transform, sprite, map);
	});
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

void Pipeline2D::RenderMap(const TransformComponent& transform, const SpriteComponent& sprite,
                           const MapComponent& map) const
{
	auto& commands = mRenderer.GetCommandBuffer();
	const auto textureId = mResourceManager.GetTextureId(sprite.mTexturePath);

	// If this assert is hit, need to add a case for commands.SetResourceHeap(*mResourceHeap);
	assert(textureId != -1);

	commands.SetResourceHeap(*mResourceHeap, textureId);

	const glm::vec2 mapTopLeft = {
		transform.mPosition.x - (map.mMapSize.x / 2) + (map.mTileSize / 2),
		transform.mPosition.y - (map.mMapSize.y / 2) + (map.mTileSize / 2)
	};
	for (int i = 0; i < map.mData.size(); i++)
	{
		const int tile = map.mData[i];
		const float posX = i % static_cast<int>(map.mColumns);
		const double currentRow = floor(i / map.mColumns);

		const glm::vec2 clipStart = {
			fmod(tile, sprite.mColumns) / sprite.mColumns, fmod(tile, sprite.mRows) / sprite.mRows
		};
		const glm::vec4 clip = {
			clipStart.x, clipStart.y, 1.0 / sprite.mColumns, 1.0 / sprite.mRows
		};

		const glm::vec3 pos = {
			mapTopLeft.x + (posX * map.mTileSize),
			mapTopLeft.y + (currentRow * map.mTileSize),
			transform.mPosition.z
		};
		const glm::vec3 size = {map.mTileSize, map.mTileSize, 0};

		_UpdateUniforms(pos, size, transform.mRotation, clip);
		commands.Draw(4, 0);
	}
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

	const SpriteSettings settings = {mRenderer.GetProjection() * mRenderer.GetView() * model, textureClip};
	commands.UpdateBuffer(*mConstantBuffer, 0, &settings, sizeof(settings));
}

void Pipeline2D::_UpdateUniforms(glm::vec3 pos, glm::vec3 size, float rot, glm::vec4 texClip) const
{
	auto& commands = mRenderer.GetCommandBuffer();
	// Update
	auto model = glm::mat4(1.0f);
	model = translate(model, pos);
	model = translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
	model = rotate(model, glm::radians(rot), glm::vec3(0.0f, 0.0f, 1.0f));
	model = translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
	model = scale(model, size);

	auto textureClip = glm::mat4(1.0f);
	textureClip = translate(textureClip, glm::vec3(texClip.x, texClip.y, 0.0f));
	textureClip = scale(textureClip, glm::vec3(texClip.z, texClip.w, 1.0f));

	const SpriteSettings settings = {mRenderer.GetProjection() * mRenderer.GetView() * model, textureClip};
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
	const auto pipelineLayout = mRenderer.GetRendererSystem()->CreatePipelineLayout(LLGL::PipelineLayoutDesc(
		"cbuffer(0):vert:frag, texture(0):frag, sampler(0):frag"));

	// Create graphics pipeline
	LLGL::GraphicsPipelineDescriptor pipelineDesc;
	{
		pipelineDesc.vertexShader = &mShader->GetVertexShader();
		pipelineDesc.fragmentShader = &mShader->GetFragmentShader();
		pipelineDesc.pipelineLayout = pipelineLayout;
		pipelineDesc.primitiveTopology = LLGL::PrimitiveTopology::TriangleStrip;
	}
	mPipeline = mRenderer.GetRendererSystem()->CreatePipelineState(pipelineDesc);

	auto textures = mResourceManager.getTextures();

	LLGL::ResourceHeapDescriptor resourceHeapDesc;
	{
		resourceHeapDesc.pipelineLayout = pipelineLayout;
		resourceHeapDesc.resourceViews.reserve(textures.size() * 3);

		for (const auto& texture : textures)
		{
			resourceHeapDesc.resourceViews.emplace_back(mConstantBuffer);
			resourceHeapDesc.resourceViews.emplace_back(&texture.second->GetTextureData());
			resourceHeapDesc.resourceViews.emplace_back(&texture.second->GetSamplerData());
		}
	}
	mResourceHeap = mRenderer.GetRendererSystem()->CreateResourceHeap(resourceHeapDesc);

	delete pipelineLayout;
}
