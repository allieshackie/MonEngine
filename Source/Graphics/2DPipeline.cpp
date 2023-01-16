#include "LLGL/Misc/VertexFormat.h"
#include "LLGL/Misc/Utility.h"

#include "Core/Renderer.h"
#include "Core/ResourceManager.h"
#include "Core/Shader.h"
#include "RenderObjects/RenderObject.h"
#include "RenderObjects/Sprite.h"
#include "Texture.h"

#include "2DPipeline.h"


Pipeline2D::Pipeline2D(Renderer& renderer, ResourceManager& resourceManager)
	: mRenderer(renderer), mResourceManager(resourceManager)
{
	_InitPipeline();
}

Pipeline2D::~Pipeline2D()
{
	delete mPipeline;
	delete mConstantBuffer;
	delete mVertexBuffer;
}

void Pipeline2D::Render(LLGL::CommandBuffer& commands) const
{
	// set graphics pipeline
	commands.SetPipelineState(*mPipeline);
	commands.SetVertexBuffer(*mVertexBuffer);

	const auto renderObjects = mResourceManager.GetDrawList();
	for (const auto obj : renderObjects)
	{
		commands.SetResourceHeap(*mResourceHeap, obj->getTextureId());
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

	mShader = std::make_unique<Shader>(*mRenderer.GetRendererSystem(), vertexFormat, "sprite.vert", "sprite.frag");

	// All layout bindings that will be used by graphics and compute pipelines
	// Create pipeline layout
	const auto pipelineLayout = mRenderer.GetRendererSystem()->CreatePipelineLayout(LLGL::PipelineLayoutDesc(
		"cbuffer(0):vert:frag, texture(0):frag, sampler(0):frag"));

	// Create graphics pipeline
	LLGL::GraphicsPipelineDescriptor pipelineDesc;
	{
		pipelineDesc.vertexShader = &mShader->GetVertexShader();
		pipelineDesc.fragmentShader = &mShader->GetFragmentShader();
		pipelineDesc.pipelineLayout = pipelineLayout;
		pipelineDesc.primitiveTopology = LLGL::PrimitiveTopology::TriangleStrip;
	}
	mPipeline = mRenderer.GetRendererSystem()->CreatePipelineState(pipelineDesc);

	auto textures = mResourceManager.getTextures();

	LLGL::ResourceHeapDescriptor resourceHeapDesc;
	{
		resourceHeapDesc.pipelineLayout = pipelineLayout;
		resourceHeapDesc.resourceViews.reserve(textures.size() * 3);

		for (const auto& texture : textures)
		{
			resourceHeapDesc.resourceViews.emplace_back(mConstantBuffer);
			resourceHeapDesc.resourceViews.emplace_back(&texture.second->GetTextureData());
			resourceHeapDesc.resourceViews.emplace_back(&texture.second->GetSamplerData());
		}
	}
	mResourceHeap = mRenderer.GetRendererSystem()->CreateResourceHeap(resourceHeapDesc);

	mVertexBuffer = mRenderer.GetRendererSystem()->CreateBuffer(
		VertexBufferDesc(static_cast<std::uint32_t>(mVertices.size() * sizeof(Vertex)), mShader->GetVertexFormat()),
		mVertices.data()
	);

	delete pipelineLayout;
}

void Pipeline2D::UpdateProjectionViewModelUniform(LLGL::CommandBuffer& commands, glm::mat4 model, glm::mat4 projection,
                                                  glm::mat4 view)
{
	spriteSettings.pvmMat = projection * view * model;
	commands.UpdateBuffer(*mConstantBuffer, 0, &spriteSettings, sizeof(spriteSettings));
}

void Pipeline2D::UpdateTextureClipUniform(LLGL::CommandBuffer& commands, glm::mat4 textureClip)
{
	spriteSettings.textureTransform = textureClip;
	commands.UpdateBuffer(*mConstantBuffer, 0, &spriteSettings, sizeof(spriteSettings));
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
