#include "LLGL/Utils/Utility.h"
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <filesystem>

#include "Entity/EntityRegistry.h"
#include "Entity/Components/MapComponent.h"
#include "Entity/Components/SpriteComponent.h"
#include "Entity/Components/TransformComponent.h"
#include "Graphics/Core/Mesh.h"
#include "Graphics/Core/ResourceManager.h"
#include "Graphics/Core/Shader.h"
#include "Graphics/Core/Vertex.h"

#include "MeshPipeline.h"

MeshPipeline::MeshPipeline(LLGL::RenderSystemPtr& renderSystem) : PipelineBase(
	renderSystem, "volume.vert", "volume.frag", LLGL::PrimitiveTopology::TriangleList)
{
	const auto meshData = ResourceManager::LoadAllMeshesFromFolder();

	for (auto& mesh : meshData)
	{
		MeshVBO vbo;
		vbo.mVertexBuffer = renderSystem->CreateBuffer(
			VertexBufferDesc(static_cast<std::uint32_t>(mesh->GetVertices().size() * sizeof(Vertex)),
			                 mShader->GetVertexFormat()), mesh->GetVertices().data());

		vbo.mIndexBuffer = renderSystem->CreateBuffer(
			LLGL::IndexBufferDesc(static_cast<std::uint32_t>(mesh->GetIndices().size() * sizeof(uint32_t)),
			                      LLGL::Format::R32UInt),
			mesh->GetIndices().data());

		vbo.mNumIndices = static_cast<int>(mesh->GetIndices().size());

		mMeshVertexBuffers[mesh->GetId()] = vbo;
	}
}

void MeshPipeline::Render(LLGL::CommandBuffer& commands, const glm::mat4 pvMat, EntityRegistry& entityRegistry)
{
	commands.SetPipelineState(*mPipeline);

	// TODO: Sort the entities based on their mesh component file names
	const auto meshView = entityRegistry.GetEnttRegistry().view<
		const TransformComponent, const MeshComponent, const SpriteComponent>(
		entt::exclude<MapComponent>);

	meshView.each([this, &commands, &pvMat](const TransformComponent& transform, const MeshComponent& mesh,
	                                        const SpriteComponent& sprite)
	{
		// Set resources
		const auto textureId = ResourceManager::GetTextureId(sprite.mTexturePath);
		commands.SetResourceHeap(*mResourceHeap, textureId);
		int numIndices = _SetMeshVBuffer(commands, mesh);

		UpdateProjectionViewModelUniform(commands, pvMat, transform);

		commands.DrawIndexed(numIndices, 0);
	});
}

void MeshPipeline::RenderMap(LLGL::CommandBuffer& commands, const glm::mat4 pvMat, const MeshComponent& meshComponent,
                             const TransformComponent& transform)
{
	int numIndices = _SetMeshVBuffer(commands, meshComponent);

	UpdateProjectionViewModelUniform(commands, pvMat, transform);

	commands.DrawIndexed(numIndices, 0);
}

void MeshPipeline::SetPipeline(LLGL::CommandBuffer& commands) const
{
	commands.SetPipelineState(*mPipeline);
}

void MeshPipeline::UpdateProjectionViewModelUniform(LLGL::CommandBuffer& commands, glm::mat4 pvMat,
                                                    const TransformComponent& transform) const
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

	const Settings settings = {pvMat * model, glm::mat4(1.0f), model};
	commands.UpdateBuffer(*mConstantBuffer, 0, &settings, sizeof(settings));
}

void MeshPipeline::SetResourceHeapTexture(LLGL::CommandBuffer& commands, int textureId) const
{
	commands.SetResourceHeap(*mResourceHeap, textureId);
}

int MeshPipeline::_SetMeshVBuffer(LLGL::CommandBuffer& commands, const MeshComponent& meshComponent)
{
	const auto meshVbo = mMeshVertexBuffers.find(meshComponent.mMeshPath);
	if (meshVbo != mMeshVertexBuffers.end())
	{
		commands.SetVertexBuffer(*meshVbo->second.mVertexBuffer);
		commands.SetIndexBuffer(*meshVbo->second.mIndexBuffer);

		return meshVbo->second.mNumIndices;
	}

	return 0;
}
