#include "LLGL/Utils/Utility.h"

#include "Graphics/Core/Shader.h"
#include "Graphics/Core/Vertex.h"

#include "MeshPipeline.h"

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

void MeshPipeline::Init(LLGL::RenderSystemPtr& renderSystem, const std::string& shaderPath)
{
	mConstantBuffer = renderSystem->CreateBuffer(LLGL::ConstantBufferDesc(sizeof(VolumeSettings)),
	                                             &volumeSettings);

	LLGL::VertexFormat vertexFormat;
	vertexFormat.AppendAttribute({"position", LLGL::Format::RGB32Float});
	vertexFormat.AppendAttribute({"normal", LLGL::Format::RGB32Float});
	vertexFormat.AppendAttribute({"texCoord", LLGL::Format::RG32Float});
	vertexFormat.SetStride(sizeof(TexturedVertex));

	std::string vertPath = shaderPath;
	vertPath.append("volume.vert");

	std::string fragPath = shaderPath;
	fragPath.append("volume.frag");

	mShader = std::make_unique<Shader>(*renderSystem, vertexFormat, vertPath.c_str(),
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
	const auto pipelineLayout = renderSystem->CreatePipelineLayout(layoutDesc);

	// Create graphics pipeline
	LLGL::GraphicsPipelineDescriptor pipelineDesc;
	{
		pipelineDesc.vertexShader = &mShader->GetVertexShader();
		pipelineDesc.fragmentShader = &mShader->GetFragmentShader();
		pipelineDesc.pipelineLayout = pipelineLayout;
		pipelineDesc.primitiveTopology = LLGL::PrimitiveTopology::TriangleStrip;
	}
	mPipeline = renderSystem->CreatePipelineState(pipelineDesc);

	std::vector<LLGL::ResourceViewDescriptor> resourceViews = {mConstantBuffer};
	mVolumeResourceHeap = renderSystem->CreateResourceHeap(pipelineLayout, resourceViews);
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
void MeshPipeline::AddRenderObjectVBuffer(const LLGL::RenderSystemPtr& renderSystem, RenderObject& obj)
{
	// TODO: Hardcoded volume test
	std::vector<TexturedVertex> vertices;
	// TODO:
	//mResourceManager.LoadObjModel(vertices, "../Data/Models/Pyramid.obj");
	model = std::make_unique<Model>();
	model->numVertices = static_cast<std::uint32_t>(vertices.size());

	mVertexBuffer = renderSystem->CreateBuffer(
		VertexBufferDesc(static_cast<std::uint32_t>(vertices.size() * sizeof(Vertex)), mShader->GetVertexFormat()),
		vertices.data()
	);
}
