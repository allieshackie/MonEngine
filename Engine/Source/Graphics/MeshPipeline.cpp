#include "LLGL/Misc/Utility.h"

#include "Core/Renderer.h"
#include "Core/ResourceManager.h"
#include "Core/Shader.h"

#include "MeshPipeline.h"

MeshPipeline::MeshPipeline(Renderer& renderer, ResourceManager& resourceManager, std::string shadersFolderPath)
	: mRenderer(renderer), mResourceManager(resourceManager), mShadersFolderPath(std::move(shadersFolderPath))
{
	_InitPipeline();
}

void MeshPipeline::Render(LLGL::CommandBuffer& commands) const
{
	// Set resources
	if (model != nullptr)
	{
		commands.SetPipelineState(*mPipeline);
		commands.SetVertexBuffer(*mVertexBuffer);
		commands.SetResourceHeap(*mVolumeResourceHeap);

		commands.Draw(model->numVertices, model->firstVertex);
	}
}

void MeshPipeline::_InitPipeline()
{
	mConstantBuffer = mRenderer.GetRendererSystem()->CreateBuffer(LLGL::ConstantBufferDesc(sizeof(VolumeSettings)),
	                                                              &volumeSettings);

	LLGL::VertexFormat vertexFormat;
	vertexFormat.AppendAttribute({"position", LLGL::Format::RGB32Float});
	vertexFormat.AppendAttribute({"normal", LLGL::Format::RGB32Float});
	vertexFormat.AppendAttribute({"texCoord", LLGL::Format::RG32Float});
	vertexFormat.SetStride(sizeof(TexturedVertex));

	std::string vertPath = mShadersFolderPath;
	vertPath.append("volume.vert");

	std::string fragPath = mShadersFolderPath;
	fragPath.append("volume.frag");

	mShader = std::make_unique<Shader>(*mRenderer.GetRendererSystem(), vertexFormat, vertPath.c_str(),
	                                   fragPath.c_str());

	// All layout bindings that will be used by graphics and compute pipelines
	LLGL::PipelineLayoutDescriptor layoutDesc;
	{
		layoutDesc.bindings =
		{
			LLGL::BindingDescriptor{
				LLGL::ResourceType::Buffer, LLGL::BindFlags::ConstantBuffer, LLGL::StageFlags::AllStages, 0
			}
		};
	}
	// Create pipeline layout
	const auto pipelineLayout = mRenderer.GetRendererSystem()->CreatePipelineLayout(layoutDesc);

	// Create graphics pipeline
	LLGL::GraphicsPipelineDescriptor pipelineDesc;
	{
		pipelineDesc.vertexShader = &mShader->GetVertexShader();
		pipelineDesc.fragmentShader = &mShader->GetFragmentShader();
		pipelineDesc.pipelineLayout = pipelineLayout;
		pipelineDesc.primitiveTopology = LLGL::PrimitiveTopology::TriangleStrip;
	}
	mPipeline = mRenderer.GetRendererSystem()->CreatePipelineState(pipelineDesc);

	// Create resource heap for constant buffer
	LLGL::ResourceHeapDescriptor heapDesc;
	{
		heapDesc.pipelineLayout = pipelineLayout;
		heapDesc.resourceViews = {mConstantBuffer};
	}
	mVolumeResourceHeap = mRenderer.GetRendererSystem()->CreateResourceHeap(heapDesc);
}

void MeshPipeline::UpdateProjectionViewModelUniform(LLGL::CommandBuffer& commands, glm::mat4 model,
                                                    glm::mat4 projection,
                                                    glm::mat4 view)
{
	// TODO: Apply rotation in ZXY order
	volumeSettings.pvmMat = projection * view * model;
	commands.UpdateBuffer(*mConstantBuffer, 0, &volumeSettings, sizeof(VolumeSettings));
}

// TODO: Implement actual model class as RenderObject
void MeshPipeline::AddRenderObjectVBuffer(RenderObject& obj)
{
	// TODO: Hardcoded volume test
	std::vector<TexturedVertex> vertices;
	mResourceManager.LoadObjModel(vertices, "../Data/Models/Pyramid.obj");
	model = std::make_unique<Model>();
	model->numVertices = static_cast<std::uint32_t>(vertices.size());

	mVertexBuffer = mRenderer.GetRendererSystem()->CreateBuffer(
		VertexBufferDesc(static_cast<std::uint32_t>(vertices.size() * sizeof(Vertex)), mShader->GetVertexFormat()),
		vertices.data()
	);
}
