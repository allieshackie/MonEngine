#include "LLGL/Misc/VertexFormat.h"
#include "LLGL/Misc/Utility.h"

#include "Core/Camera.h"
#include "Core/LevelManager.h"
#include "Core/ResourceManager.h"
#include "Entity/EntityRegistry.h"
#include "Entity/Components/MapComponent.h"
#include "Entity/Components/TransformComponent.h"
#include "Entity/Components/SpriteComponent.h"
#include "Texture.h"

#include "SpritePipeline.h"

void SpritePipeline::Render(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat,
                            EntityRegistry& entityRegistry) const
{
	// set graphics pipeline
	commandBuffer.SetPipelineState(*mPipeline);
	commandBuffer.SetVertexBuffer(*mVertexBuffer);

	const auto spriteView = entityRegistry.GetEnttRegistry().view<const TransformComponent, const SpriteComponent>(
		entt::exclude<MapComponent>);
	spriteView.each([this, &commandBuffer, &pvMat](const TransformComponent& transform, const SpriteComponent& sprite)
	{
		_Render(commandBuffer, pvMat, transform, sprite);
	});
}

void SpritePipeline::_Render(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat,
                             const TransformComponent& transform, const SpriteComponent& sprite) const
{
	//const auto textureId = mResourceManager.GetTextureId(sprite.mTexturePath);

	commandBuffer.SetResourceHeap(*mResourceHeap, sprite.mTextureId);

	_UpdateUniforms(commandBuffer, pvMat, transform);

	commandBuffer.Draw(4, 0);
}

void SpritePipeline::_UpdateUniforms(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat,
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

void SpritePipeline::Init(std::unique_ptr<LLGL::RenderSystem>& renderSystem, const std::string& shaderPath,
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

	_CreateResourceHeap(renderSystem, textures);

	mVertexBuffer = renderSystem->CreateBuffer(
		VertexBufferDesc(static_cast<std::uint32_t>(mVertices.size() * sizeof(Vertex)),
		                 mShader->GetVertexFormat()),
		mVertices.data()
	);
}

void SpritePipeline::_CreateResourceHeap(const std::unique_ptr<LLGL::RenderSystem>& renderSystem,
                                         const TextureMap& textures)
{
	// Resource Heap
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
	mResourceHeap = renderSystem->CreateResourceHeap(resourceHeapDesc);
}
