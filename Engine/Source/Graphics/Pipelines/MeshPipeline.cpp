#include "LLGL/Utils/Parse.h"
#include "LLGL/Utils/Utility.h"
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <filesystem>

#include "Graphics/Core/Mesh.h"
#include "Graphics/Core/ResourceManager.h"
#include "Graphics/Core/Shader.h"
#include "Graphics/Core/Vertex.h"

#include "MeshPipeline.h"

void MeshPipeline::Render(LLGL::CommandBuffer& commands, const glm::mat4 pvMat)
{
	// Set resources
	commands.SetPipelineState(*mPipeline);
	commands.SetVertexBuffer(*mVertexBuffer);
	commands.SetIndexBuffer(*mIndexBuffer);
	commands.SetResourceHeap(*mVolumeResourceHeap, 1);

	UpdateProjectionViewModelUniform(commands, glm::mat4(), pvMat);

	commands.DrawIndexed(36, 0);
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

	auto vertices = ResourceManager::GenerateTexturedCubeVertices();
	auto indices = ResourceManager::GenerateTexturedCubeTriangleIndices();

	mVertexBuffer = renderSystem->CreateBuffer(
		VertexBufferDesc(static_cast<std::uint32_t>(vertices.size() * sizeof(TexturedVertex)),
		                 mShader->GetVertexFormat()), vertices.data()
	);

	mIndexBuffer = renderSystem->CreateBuffer(
		IndexBufferDesc(static_cast<std::uint32_t>(indices.size() * sizeof(std::uint32_t)),
		                LLGL::Format::R32UInt), indices.data()
	);
}

void MeshPipeline::UpdateProjectionViewModelUniform(LLGL::CommandBuffer& commands, glm::mat4 model, glm::mat4 pvMat)
{
	// TODO: Apply rotation in ZXY order
	//mVolumeSettings.pvmMat = projection * view * model;

	float rotation = glm::radians(-20.0f);
	mVolumeSettings.mMat = glm::identity<glm::mat4>();
	glm::translate(mVolumeSettings.mMat, glm::vec3{0, 0, 5});
	glm::rotate(mVolumeSettings.mMat, rotation, glm::vec3{0, 1, 0});

	mVolumeSettings.pvmMat = pvMat;
	mVolumeSettings.pvmMat *= mVolumeSettings.mMat;

	commands.UpdateBuffer(*mConstantBuffer, 0, &mVolumeSettings, sizeof(VolumeSettings));
}

void MeshPipeline::AddRenderObjectVBuffer(const LLGL::RenderSystemPtr& renderSystem, const Mesh& mesh)
{
	mVertexBuffer = renderSystem->CreateBuffer(
		VertexBufferDesc(static_cast<std::uint32_t>(mesh.GetNumVertices() * sizeof(TexturedVertex)),
		                 mShader->GetVertexFormat()), mesh.GetVertices().data()
	);
}
