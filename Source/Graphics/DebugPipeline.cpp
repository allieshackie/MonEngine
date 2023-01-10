#include "LLGL/Misc/Utility.h"
#include "Core/Renderer.h"
#include "Core/ResourceManager.h"
#include "Core/Shader.h"
#include "RenderObjects/DebugDraw.h"

#include "DebugPipeline.h"

DebugPipeline::DebugPipeline(Renderer& renderer, ResourceManager& resourceManager)
	: mRenderer(renderer), mResourceManager(resourceManager)
{
	_InitPipeline();
}

void DebugPipeline::Render(LLGL::CommandBuffer& commands) const
{
	// set graphics pipeline
	commands.SetPipelineState(*mPipeline);
	commands.SetResourceHeap(*mResourceHeap);

	const auto renderObjects = mResourceManager.GetDebugDrawList();
	for (const auto obj : renderObjects)
	{
		commands.SetVertexBuffer(obj->GetVertexBuffer());
		obj->Draw(mRenderer, commands);
	}
}

void DebugPipeline::UpdateProjectionViewModelUniform(LLGL::CommandBuffer& commands, glm::mat4 model,
                                                     glm::mat4 projection, glm::mat4 view)
{
	settings.pvmMat = projection * view * model;
	commands.UpdateBuffer(*mConstantBuffer, 0, &settings, sizeof(settings));
}

void DebugPipeline::ClearDebugDraw() const
{
	mResourceManager.ClearDebugDrawList();
}

void DebugPipeline::AddRenderObjectVBuffer(RenderObject& obj) const
{
	if (!obj.GetVertices().empty())
	{
		obj.SetVertexBuffer(
			mRenderer.GetRendererSystem()->CreateBuffer(
				VertexBufferDesc(static_cast<std::uint32_t>(obj.GetVertices().size() * sizeof(Vertex)),
				                 mShader->GetVertexFormat()),
				obj.GetVertices().data()));
	}
}

void DebugPipeline::_InitPipeline()
{
	mConstantBuffer = mRenderer.GetRendererSystem()->CreateBuffer(LLGL::ConstantBufferDesc(sizeof(Settings)),
	                                                              &settings);
	LLGL::VertexFormat vertexFormat;
	vertexFormat.AppendAttribute({"position", LLGL::Format::RGB32Float});
	vertexFormat.AppendAttribute({"color", LLGL::Format::RGB32Float});
	vertexFormat.AppendAttribute({"texCoord", LLGL::Format::RG32Float});
	vertexFormat.SetStride(sizeof(Vertex));

	mShader = new Shader(*mRenderer.GetRendererSystem(), vertexFormat, "debug.vert", "debug.frag");

	// Create pipeline layout
	const auto pipelineLayout = mRenderer.GetRendererSystem()->CreatePipelineLayout(LLGL::PipelineLayoutDesc(
		"cbuffer(0):vert:frag"));

	// Create graphics pipeline
	LLGL::GraphicsPipelineDescriptor pipelineDesc;
	{
		pipelineDesc.vertexShader = &mShader->GetVertexShader();
		pipelineDesc.fragmentShader = &mShader->GetFragmentShader();
		pipelineDesc.primitiveTopology = LLGL::PrimitiveTopology::LineList;
	}
	mPipeline = mRenderer.GetRendererSystem()->CreatePipelineState(pipelineDesc);

	auto textures = mResourceManager.getTextures();

	LLGL::ResourceHeapDescriptor resourceHeapDesc;
	{
		resourceHeapDesc.pipelineLayout = pipelineLayout;
		resourceHeapDesc.resourceViews = {mConstantBuffer};
	}
	mResourceHeap = mRenderer.GetRendererSystem()->CreateResourceHeap(resourceHeapDesc);
}
