#include "OverlayPipeline.h"

#include "Util/gltfHelpers.h"

void OverlayPipeline::Render(LLGL::CommandBuffer& commands, const glm::mat4 projMat)
{
	if (mOverlayVertices.empty() && mOverlayElements.empty())
	{
		return;
	}
	// set graphics pipeline
	commands.SetPipelineState(*mPipeline);
	commands.SetResourceHeap(*mResourceHeap);

	if (mNumOverlayVertices != mOverlayVertices.size())
	{
		commands.UpdateBuffer(*mOverlayVertexBuffer, 0, mOverlayVertices.data(),
		                      static_cast<std::uint16_t>(mOverlayVertices.size() * sizeof(DebugVertex)));
		mNumOverlayVertices = mOverlayVertices.size();
	}

	if (!mOverlayVertices.empty())
	{
		overlaySettings.pvmMat = projMat;
		commands.UpdateBuffer(*mConstantBuffer, 0, &overlaySettings, sizeof(overlaySettings));
		commands.SetVertexBuffer(*mOverlayVertexBuffer);
		commands.Draw(mOverlayVertices.size(), 0);
	}

	for (const auto& element : mOverlayElements)
	{
		DrawOverlay(*element, commands, projMat);
	}
}

int OverlayPipeline::AddOverlay(const std::vector<DebugVertex>& vertices, glm::mat4 transform)
{
	auto element = std::make_unique<OverlayElement>();
	element->vertices = vertices;
	element->transform = transform;
	element->mVertexBuffer = mRenderSystem->CreateBuffer(
		LLGL::VertexBufferDesc(static_cast<std::uint32_t>(element->vertices.size() * sizeof(DebugVertex)),
		                       mShader->GetVertexFormat()), element->vertices.data());
	int id = static_cast<int>(mOverlayElements.size());
	element->mId = id;

	mOverlayElements.push_back(std::move(element));

	return id;
}

void OverlayPipeline::UpdateOverlayTransform(int id, glm::mat4 transform) const
{
	mOverlayElements[id]->transform = transform;
}

void OverlayPipeline::DrawOverlay(const OverlayElement& element, LLGL::CommandBuffer& commands, const glm::mat4 projMat)
{
	overlaySettings.pvmMat = projMat * element.transform;
	commands.UpdateBuffer(*mConstantBuffer, 0, &overlaySettings, sizeof(overlaySettings));

	commands.SetVertexBuffer(*element.mVertexBuffer);
	commands.Draw(element.vertices.size(), 0);
}

void OverlayPipeline::DrawLine(glm::vec3 from, glm::vec3 to, glm::vec4 color)
{
	mOverlayVertices.push_back({from, color});
	mOverlayVertices.push_back({to, color});
}

OverlayPipeline::OverlayPipeline(const LLGL::RenderSystemPtr& renderSystem)
	: PipelineBase(), mRenderSystem(renderSystem)
{
	// Initialization
	{
		InitConstantBuffer<OverlaySettings>(renderSystem, overlaySettings);

		LLGL::VertexFormat vertexFormat;
		vertexFormat.AppendAttribute({"position", LLGL::Format::RGB32Float, 0, 0, sizeof(DebugVertex), 0});
		vertexFormat.AppendAttribute({"color", LLGL::Format::RGBA32Float, 1, 12, sizeof(DebugVertex), 0});

		InitShader(renderSystem, vertexFormat, "overlay.vert", "overlay.frag");
		LLGL::PipelineLayoutDescriptor layoutDesc;
		{
			layoutDesc.heapBindings =
			{
				LLGL::BindingDescriptor{
					"OverlaySettings", LLGL::ResourceType::Buffer, LLGL::BindFlags::ConstantBuffer,
					LLGL::StageFlags::VertexStage, 0
				},
			};
		}
		InitPipeline(renderSystem, layoutDesc, LLGL::PrimitiveTopology::LineList, false);

		mOverlayVertexBuffer = renderSystem->CreateBuffer(
			VertexBufferDesc(static_cast<std::uint32_t>(50 * sizeof(DebugVertex)),
			                 mShader->GetVertexFormat()));

		const std::vector<LLGL::ResourceViewDescriptor> resourceViews = {
			mConstantBuffer
		};
		InitResourceHeap(renderSystem, resourceViews);
	}
}
