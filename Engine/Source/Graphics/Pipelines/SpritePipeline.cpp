#include "LLGL/Utils/VertexFormat.h"
#include "LLGL/Utils/Utility.h"

#include "Core/Camera.h"
#include "Graphics/Core/ResourceManager.h"
#include "Graphics/Core/Shader.h"
#include "Graphics/Core/Texture.h"
#include "Entity/EntityRegistry.h"
#include "Entity/Components/MapComponent.h"
#include "Entity/Components/SpriteComponent.h"
#include "Entity/Components/TransformComponent.h"

#include "SpritePipeline.h"

SpritePipeline::SpritePipeline(const LLGL::RenderSystemPtr& renderSystem) : PipelineBase(
	renderSystem, "sprite.vert", "sprite.frag")
{
	mVertexBuffer = renderSystem->CreateBuffer(
		VertexBufferDesc(static_cast<std::uint32_t>(mVertices.size() * sizeof(Vertex)),
		                 mShader->GetVertexFormat()), mVertices.data()
	);
}

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
	const auto textureId = ResourceManager::GetTextureId(sprite.mTexturePath);
	commandBuffer.SetResourceHeap(*mResourceHeap, textureId);

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

	const Settings settings = {pvMat * model, glm::mat4(1.0f), glm::mat4()};
	commandBuffer.UpdateBuffer(*mConstantBuffer, 0, &settings, sizeof(settings));
}
