#include <filesystem>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

#include "Core/Camera.h"
#include "Entity/EntityRegistry.h"
#include "Entity/Components/MapComponent.h"
#include "Entity/Components/SpriteComponent.h"
#include "Entity/Components/TransformComponent.h"
#include "Graphics/Core/ResourceManager.h"

#include "MeshPipeline.h"

MeshPipeline::MeshPipeline(LLGL::RenderSystemPtr& renderSystem) : PipelineBase(
	renderSystem, "mesh.vert", "mesh.frag", LLGL::PrimitiveTopology::TriangleList)
{
	for (const auto& entry : std::filesystem::directory_iterator(MODELS_FOLDER))
	{
		auto fullPath = entry.path().string();
		auto model = Model(fullPath, entry.path().filename().string());
		model.InitializeBuffers(renderSystem, *mShader);
		mModelVertexBuffers[model.GetId()] = model;
	}
}

void MeshPipeline::Render(LLGL::CommandBuffer& commands, const Camera& camera, const glm::mat4 projection,
                          EntityRegistry& entityRegistry)
{
	commands.SetPipelineState(*mPipeline);

	// TODO: Sort the entities based on their mesh component file names
	const auto meshView = entityRegistry.GetEnttRegistry().view<
		const TransformComponent, const MeshComponent, const SpriteComponent>(
		entt::exclude<MapComponent>);

	meshView.each([this, &commands, &camera, &projection](const TransformComponent& transform,
	                                                      const MeshComponent& mesh,
	                                                      const SpriteComponent& sprite)
	{
		// Set resources
		const auto textureId = ResourceManager::GetTextureId(sprite.mTexturePath);
		commands.SetResourceHeap(*mResourceHeap, textureId);
		_RenderModel(commands, mesh, camera, projection, transform);
	});
}

void MeshPipeline::RenderMap(LLGL::CommandBuffer& commands, const Camera& camera, const glm::mat4 projection,
                             const MeshComponent& meshComponent, const TransformComponent& transform)
{
	_RenderModel(commands, meshComponent, camera, projection, transform);
}

void MeshPipeline::SetPipeline(LLGL::CommandBuffer& commands) const
{
	commands.SetPipelineState(*mPipeline);
}

void MeshPipeline::UpdateProjectionViewModelUniform(LLGL::CommandBuffer& commands, const Camera& camera,
                                                    const glm::mat4 projection, const TransformComponent& transform)
{
	// Update
	auto model = glm::mat4(1.0f);

	// Translation matrix
	model = glm::translate(model, transform.mPosition);

	// Apply rotation in ZXY order
	model = glm::rotate(model, glm::radians(transform.mRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(transform.mRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(transform.mRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));

	model = glm::scale(model, transform.mSize);

	settings.model = model;
	settings.view = camera.GetView();
	settings.projection = projection;
	settings.textureClip = glm::mat4(1.0f);

	settings.viewPos = camera.GetPosition();

	commands.UpdateBuffer(*mConstantBuffer, 0, &settings, sizeof(settings));
}

void MeshPipeline::SetResourceHeapTexture(LLGL::CommandBuffer& commands, int textureId) const
{
	commands.SetResourceHeap(*mResourceHeap, textureId);
}

void MeshPipeline::_RenderModel(LLGL::CommandBuffer& commands, const MeshComponent& meshComponent, const Camera& camera,
                                const glm::mat4 projection, const TransformComponent& transform)
{
	UpdateProjectionViewModelUniform(commands, camera, projection, transform);
	const auto model = mModelVertexBuffers.find(meshComponent.mMeshPath);
	if (model != mModelVertexBuffers.end())
	{
		for (const auto& mesh : model->second.GetMeshes())
		{
			commands.SetVertexBuffer(*mesh.mVertexBuffer);
			commands.SetIndexBuffer(*mesh.mIndexBuffer);
			commands.DrawIndexed(mesh.mNumIndices, 0);
		}
	}
}
