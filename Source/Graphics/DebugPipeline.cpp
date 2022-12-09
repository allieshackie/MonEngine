#include "LLGL/Utility.h"
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

	const auto renderObjects = mResourceManager.GetDebugDrawList();
	for (const auto obj : renderObjects)
	{
		commands.SetVertexBuffer(obj->GetVertexBuffer());
		obj->Draw(mRenderer, commands);
	}
}

void DebugPipeline::UpdateProjectionViewModelUniform(LLGL::CommandBuffer& commands, glm::mat4 model,
                                                     glm::mat4 projection, glm::mat4 view) const
{
	const LLGL::UniformLocation pvmUniform = mShader->GetShaderProgram().FindUniformLocation("pvmMat");
	const auto pvmMat = projection * view * model;
	commands.SetUniform(pvmUniform, &pvmMat, sizeof(pvmMat));
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
	LLGL::VertexFormat vertexFormat;
	vertexFormat.AppendAttribute({"position", LLGL::Format::RGB32Float});
	vertexFormat.AppendAttribute({"color", LLGL::Format::RGB32Float});
	vertexFormat.AppendAttribute({"texCoord", LLGL::Format::RG32Float});

	mShader = new Shader(*mRenderer.GetRendererSystem(), vertexFormat, "debug.vert", "debug.frag");

	// Create graphics pipeline
	LLGL::GraphicsPipelineDescriptor pipelineDesc;
	{
		pipelineDesc.shaderProgram = &mShader->GetShaderProgram();
		pipelineDesc.primitiveTopology = LLGL::PrimitiveTopology::LineList;
	}
	mPipeline = mRenderer.GetRendererSystem()->CreatePipelineState(pipelineDesc);
}
