#include "LLGL/Utils/Parse.h"
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
		commands.SetResourceHeap(*mVolumeResourceHeap, textureId);
		int numIndices = _SetMeshVBuffer(commands, mesh);

		UpdateProjectionViewModelUniform(commands, pvMat, transform);

		commands.DrawIndexed(numIndices, 0);
	});
}

void MeshPipeline::RenderMap(LLGL::CommandBuffer& commands, const glm::mat4 pvMat, const MeshComponent& meshComponent,
                             const TransformComponent& transform)
{
	//commands.SetResourceHeap(*mVolumeResourceHeap, 1);

	int numIndices = _SetMeshVBuffer(commands, meshComponent);

	UpdateProjectionViewModelUniform(commands, pvMat, transform);

	commands.DrawIndexed(numIndices, 0);
}

void MeshPipeline::SetPipeline(LLGL::CommandBuffer& commands) const
{
	commands.SetPipelineState(*mPipeline);
}

void MeshPipeline::Init(LLGL::RenderSystemPtr& renderSystem)
{
	mConstantBuffer = renderSystem->CreateBuffer(LLGL::ConstantBufferDesc(sizeof(VolumeSettings)),
	                                             &mVolumeSettings);

	LLGL::VertexFormat vertexFormat;
	vertexFormat.AppendAttribute({"position", LLGL::Format::RGB32Float});
	vertexFormat.AppendAttribute({"normal", LLGL::Format::RGB32Float});
	vertexFormat.AppendAttribute({"texCoord", LLGL::Format::RG32Float});

	std::string vertPath = SHADERS_FOLDER;
	vertPath.append("volume.vert");

	std::string fragPath = SHADERS_FOLDER;
	fragPath.append("volume.frag");

	mShader = std::make_unique<Shader>(*renderSystem, vertexFormat, vertPath.c_str(),
	                                   fragPath.c_str());

	auto pipelineLayout = renderSystem->CreatePipelineLayout(
		LLGL::Parse("heap{cbuffer(0):vert:frag, texture(0):frag, sampler(0):frag}"));

	// Create graphics pipeline
	LLGL::GraphicsPipelineDescriptor pipelineDesc;
	{
		pipelineDesc.vertexShader = &mShader->GetVertexShader();
		pipelineDesc.fragmentShader = &mShader->GetFragmentShader();
		pipelineDesc.pipelineLayout = pipelineLayout;
		pipelineDesc.primitiveTopology = LLGL::PrimitiveTopology::TriangleList;
	}
	mPipeline = renderSystem->CreatePipelineState(pipelineDesc);

	// Resource Heap
	const auto textures = ResourceManager::LoadAllTexturesFromFolder(renderSystem);

	std::vector<LLGL::ResourceViewDescriptor> resourceViews;
	resourceViews.reserve(textures.size() * 3);

	for (const auto& texture : textures)
	{
		resourceViews.emplace_back(mConstantBuffer);
		resourceViews.emplace_back(&texture->GetTextureData());
		resourceViews.emplace_back(&texture->GetSamplerData());
	}

	mVolumeResourceHeap = renderSystem->CreateResourceHeap(pipelineLayout, resourceViews);

	const auto meshData = ResourceManager::LoadAllMeshesFromFolder();

	for (auto& mesh : meshData)
	{
		MeshVBO vbo;
		vbo.mVertexBuffer = renderSystem->CreateBuffer(
			VertexBufferDesc(static_cast<std::uint32_t>(mesh->GetVertices().size() * sizeof(TexturedVertex)),
			                 mShader->GetVertexFormat()), mesh->GetVertices().data());

		vbo.mIndexBuffer = renderSystem->CreateBuffer(
			IndexBufferDesc(static_cast<std::uint32_t>(mesh->GetIndices().size() * sizeof(std::uint32_t)),
			                LLGL::Format::R32UInt), mesh->GetIndices().data());

		vbo.mNumIndices = static_cast<int>(mesh->GetIndices().size());

		mMeshVertexBuffers[mesh->GetId()] = vbo;
	}

	auto vertices = ResourceManager::GenerateTexturedCubeVertices();
	mTestMesh.mVertexBuffer = renderSystem->CreateBuffer(
		VertexBufferDesc(static_cast<std::uint32_t>(vertices.size() * sizeof(TexturedVertex)),
		                 mShader->GetVertexFormat()), vertices.data());

	auto indices = ResourceManager::GenerateTexturedCubeTriangleIndices();
	mTestMesh.mIndexBuffer = renderSystem->CreateBuffer(
		IndexBufferDesc(static_cast<std::uint32_t>(indices.size() * sizeof(std::uint32_t)),
		                LLGL::Format::R32UInt), indices.data());

	mTestMesh.mNumIndices = static_cast<int>(indices.size());
}

void MeshPipeline::UpdateProjectionViewModelUniform(LLGL::CommandBuffer& commands, glm::mat4 pvMat,
                                                    const TransformComponent& transform) const
{
	// Update
	auto model = glm::mat4(1.0f);
	model = glm::translate(model, transform.mPosition);
	model = glm::translate(model, glm::vec3(0.5f * transform.mSize.x, 0.5f * transform.mSize.y, 0.0f));
	// Apply rotation in ZXY order
	model = glm::rotate(model, glm::radians(transform.mRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(transform.mRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(transform.mRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));

	model = glm::translate(model, glm::vec3(-0.5f * transform.mSize.x, -0.5f * transform.mSize.y, 0.0f));
	model = glm::scale(model, transform.mSize);

	const VolumeSettings settings = {pvMat * model, model};
	commands.UpdateBuffer(*mConstantBuffer, 0, &settings, sizeof(settings));
}

int MeshPipeline::_SetMeshVBuffer(LLGL::CommandBuffer& commands, const MeshComponent& meshComponent)
{
	/*
	 *
	const auto meshVbo = mMeshVertexBuffers.find(meshComponent.mMeshPath);
	if (meshVbo != mMeshVertexBuffers.end())
	{
		commands.SetVertexBuffer(*meshVbo->second.mVertexBuffer);
		commands.SetIndexBuffer(*meshVbo->second.mIndexBuffer);

		return meshVbo->second.mNumIndices;
	}
	 */

	//return 0;

	commands.SetVertexBuffer(*mTestMesh.mVertexBuffer);
	commands.SetIndexBuffer(*mTestMesh.mIndexBuffer);

	return mTestMesh.mNumIndices;
}
