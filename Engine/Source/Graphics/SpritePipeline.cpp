#include "LLGL/Misc/VertexFormat.h"
#include "LLGL/Misc/Utility.h"

#include "Core/Camera.h"
#include "Core/LevelManager.h"
#include "Core/Renderer.h"
#include "Core/ResourceManager.h"
#include "Entity/EntityRegistry.h"
#include "Entity/Components/MapComponent.h"
#include "Entity/Components/TransformComponent.h"
#include "Entity/Components/SpriteComponent.h"
#include "Texture.h"

#include "SpritePipeline.h"

SpritePipeline::SpritePipeline(EntityRegistry& entityRegistry, LevelManager& levelManager, Renderer& renderer,
                               ResourceManager& resourceManager, std::string shadersFolderPath)
	: mEntityRegistry(entityRegistry), mLevelManager(levelManager), mRenderer(renderer),
	  mResourceManager(resourceManager), mShadersFolderPath(std::move(shadersFolderPath))
{
	_InitPipeline();

	mVertexBuffer = mRenderer.GetRendererSystem()->CreateBuffer(
		VertexBufferDesc(static_cast<std::uint32_t>(mVertices.size() * sizeof(Vertex)),
		                 mShader->GetVertexFormat()),
		mVertices.data()
	);
}

void SpritePipeline::Tick() const
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
}

void SpritePipeline::Render(const TransformComponent& transform, const SpriteComponent& sprite) const
{
	auto& commands = mRenderer.GetCommandBuffer();
	const auto textureId = mResourceManager.GetTextureId(sprite.mTexturePath);

	// If this assert is hit, need to add a case for commands.SetResourceHeap(*mResourceHeap);
	assert(textureId != -1);

	commands.SetResourceHeap(*mResourceHeap, textureId);

	_UpdateUniforms(transform);

	commands.Draw(4, 0);
}

void SpritePipeline::_UpdateUniforms(const TransformComponent& transform) const
{
	auto& commands = mRenderer.GetCommandBuffer();
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

	if (const auto& level = mLevelManager.GetCurrentLevel())
	{
		const auto& camera = level->GetCamera();
		const SpriteSettings settings = {mRenderer.GetProjection() * camera->GetView() * model, textureClip};
		commands.UpdateBuffer(*mConstantBuffer, 0, &settings, sizeof(settings));
	}
}

void SpritePipeline::_InitPipeline()
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

void SpritePipeline::_CreateResourceHeap()
{
	// Resource Heap
	const auto& textures = mResourceManager.getTextures();

	LLGL::ResourceHeapDescriptor resourceHeapDesc;
	{
		resourceHeapDesc.pipelineLayout = mPipelineLayout;
		resourceHeapDesc.resourceViews.reserve(textures.size() * 3);

		for (const auto& texture : textures)
		{
			resourceHeapDesc.resourceViews.emplace_back(mConstantBuffer);
			resourceHeapDesc.resourceViews.emplace_back(&texture.second->GetTextureData());
			resourceHeapDesc.resourceViews.emplace_back(&texture.second->GetSamplerData());
		}
	}
	mResourceHeap = mRenderer.GetRendererSystem()->CreateResourceHeap(resourceHeapDesc);
}
