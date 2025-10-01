#include "OverlayPipeline.h"

#include "Util/gltfHelpers.h"

void OverlayPipeline::Render(LLGL::CommandBuffer& commands)
{
	if (mOverlayElements.empty())
	{
		return;
	}
	// set graphics pipeline
	commands.SetPipelineState(*mPipeline);

	if (mNumOverlayElements != mOverlayElements.size())
	{
		commands.UpdateBuffer(*mOverlayVertexBuffer, 0, mOverlayElements.data(),
		                      static_cast<std::uint16_t>(mOverlayElements.size() * sizeof(OverlayVertex)));
		mNumOverlayElements = mOverlayElements.size();
	}

	if (!mOverlayElements.empty())
	{
		commands.SetVertexBuffer(*mOverlayVertexBuffer);
		commands.Draw(mOverlayElements.size(), 0);
	}
}

void OverlayPipeline::DrawOverlay(glm::vec2 pos, glm::vec4 color)
{
	mOverlayElements.push_back({pos * mBoxVertices[0], color});
	mOverlayElements.push_back({pos * mBoxVertices[1], color});
	mOverlayElements.push_back({pos * mBoxVertices[3], color});

	mOverlayElements.push_back({pos * mBoxVertices[2], color});
	mOverlayElements.push_back({pos * mBoxVertices[3], color});
	mOverlayElements.push_back({pos * mBoxVertices[0], color});
}

void OverlayPipeline::DrawLine(glm::vec2 from, glm::vec2 to, glm::vec4 color)
{
	mOverlayElements.push_back({from, color});
	mOverlayElements.push_back({to, color});
}

OverlayPipeline::OverlayPipeline(const LLGL::RenderSystemPtr& renderSystem) : PipelineBase()
{
	// Initialization
	{
		LLGL::VertexFormat vertexFormat;
		vertexFormat.AppendAttribute({"position", LLGL::Format::RG32Float, 0, 0, sizeof(OverlayVertex), 0});
		vertexFormat.AppendAttribute({"color", LLGL::Format::RGBA32Float, 1, 8, sizeof(OverlayVertex), 0});

		InitShader(renderSystem, vertexFormat, "overlay.vert", "overlay.frag");
		LLGL::PipelineLayoutDescriptor layoutDesc;
		InitPipeline(renderSystem, layoutDesc, LLGL::PrimitiveTopology::TriangleList, false);

		mOverlayVertexBuffer = renderSystem->CreateBuffer(
			VertexBufferDesc(static_cast<std::uint32_t>(50 * sizeof(OverlayVertex)),
			                 mShader->GetVertexFormat()));
	}
}
