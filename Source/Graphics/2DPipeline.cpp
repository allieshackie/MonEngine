#include "LLGL/Utility.h"

#include "Core/Renderer.h"
#include "Core/ResourceManager.h"
#include "Core/Shader.h"
#include "RenderObjects/RenderObject.h"

#include "2DPipeline.h"

Pipeline2D::Pipeline2D(Renderer& renderer, ResourceManager& resourceManager)
	: mRenderer(renderer), mResourceManager(resourceManager)
{
	_InitPipeline();
}

void Pipeline2D::Render(LLGL::CommandBuffer& commands) const
{
	// set graphics pipeline
	commands.SetPipelineState(*mPipeline);
	commands.SetVertexBuffer(*mVertexBuffer);

	const auto renderObjects = mResourceManager.GetDrawList();
	for (const auto obj : renderObjects)
	{
		obj->Draw(mRenderer, commands);
	}
}

void Pipeline2D::_InitPipeline()
{
	mConstantBuffer = mRenderer.GetRendererSystem()->CreateBuffer(LLGL::ConstantBufferDesc(sizeof(SpriteSettings)),
	                                                              &spriteSettings);

	LLGL::VertexFormat vertexFormat;
	vertexFormat.AppendAttribute({"position", LLGL::Format::RGB32Float});
	vertexFormat.AppendAttribute({"color", LLGL::Format::RGB32Float});
	vertexFormat.AppendAttribute({"texCoord", LLGL::Format::RG32Float});

	mShader = new Shader(*mRenderer.GetRendererSystem(), vertexFormat, "sprite.vert", "sprite.frag");

	// All layout bindings that will be used by graphics and compute pipelines
	LLGL::PipelineLayoutDescriptor layoutDesc;
	{
		layoutDesc.bindings =
		{
			LLGL::BindingDescriptor{
				LLGL::ResourceType::Buffer, LLGL::BindFlags::ConstantBuffer, LLGL::StageFlags::AllStages, 0
			},
			LLGL::BindingDescriptor{
				LLGL::ResourceType::Texture, LLGL::BindFlags::Sampled, LLGL::StageFlags::FragmentStage, 0u
			},
			LLGL::BindingDescriptor{LLGL::ResourceType::Sampler, 0, LLGL::StageFlags::FragmentStage, 0},
		};
	}
	// Create pipeline layout
	const auto pipelineLayout = mRenderer.GetRendererSystem()->CreatePipelineLayout(layoutDesc);

	// Create graphics pipeline
	LLGL::GraphicsPipelineDescriptor pipelineDesc;
	{
		pipelineDesc.shaderProgram = &mShader->GetShaderProgram();
		pipelineDesc.pipelineLayout = pipelineLayout;
		pipelineDesc.primitiveTopology = LLGL::PrimitiveTopology::TriangleStrip;
	}
	mPipeline = mRenderer.GetRendererSystem()->CreatePipelineState(pipelineDesc);

	mResourceManager.CreateResourceHeap(*mRenderer.GetRendererSystem(), *pipelineLayout, *mConstantBuffer);

	mVertexBuffer = mRenderer.GetRendererSystem()->CreateBuffer(
		VertexBufferDesc(static_cast<std::uint32_t>(mVertices.size() * sizeof(Vertex)), mShader->GetVertexFormat()),
		mVertices.data()
	);
}

void Pipeline2D::UpdateProjectionViewModelUniform(LLGL::CommandBuffer& commands, glm::mat4 model, glm::mat4 projection,
                                                  glm::mat4 view)
{
	spriteSettings.pvmMat = projection * view * model;
	commands.UpdateBuffer(*mConstantBuffer, 0, &spriteSettings, sizeof(SpriteSettings));
}

// TODO: We don't need individual vertex buffers since they all share one
void Pipeline2D::AddRenderObjectVBuffer(RenderObject& obj) const
{
	/*
	 *
	obj.SetVertexBuffer(Renderer::GetInstance()->GetRendererSystem()->CreateBuffer(
		VertexBufferDesc(static_cast<std::uint32_t>(mVertices.size() * sizeof(Vertex)), mShader->GetVertexFormat()),
		mVertices.data()
	));
	 */
}

void Pipeline2D::UpdateTextureClipUniform(LLGL::CommandBuffer& commands, glm::mat4 textureClip)
{
	spriteSettings.textureClip = textureClip;
	commands.UpdateBuffer(*mConstantBuffer, 0, &spriteSettings, sizeof(SpriteSettings));
}
