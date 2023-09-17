#include "LLGL/Misc/Utility.h"
#include "Graphics/Core/Vertex.h"
#include "Graphics/Debug/DebugDraw.h"

#include "DebugPipeline.h"

void DebugPipeline::Render(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat)
{
	// If point size change is needed, confirm works 
	//glPointSize(5.0);
	if (mDebugDrawManager->HasUpdated())
	{
		const auto& linesList = mDebugDrawManager->GetLineList();

		commandBuffer.UpdateBuffer(*mVertexBuffer, 0, linesList.data(),
		                           static_cast<std::uint16_t>(linesList.size() * sizeof(
			                           DebugVertex)));

		mDebugDrawManager->ResetUpdate();
	}

	// set graphics pipeline
	commandBuffer.SetPipelineState(*mPipeline);
	// TODO: Do we need a Resource heap to use constant buffer? Don't think so
	//commandBuffer.SetResourceHeap(*mResourceHeap);
	commandBuffer.SetResource(*mConstantBuffer, 0, LLGL::BindFlags::ConstantBuffer, LLGL::StageFlags::VertexStage);
	commandBuffer.SetVertexBuffer(*mVertexBuffer);

	UpdateProjectionViewUniform(commandBuffer, pvMat);

	const auto& lineList = mDebugDrawManager->GetLineList();
	commandBuffer.Draw(lineList.size(), 0);

	mDebugDrawManager->ClearList();
}

void DebugPipeline::UpdateProjectionViewUniform(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat)
{
	settings.pvMat = pvMat;
	commandBuffer.UpdateBuffer(*mConstantBuffer, 0, &settings, sizeof(settings));
}

void DebugPipeline::Init(std::shared_ptr<LLGL::RenderSystem>& renderSystem)
{
	mConstantBuffer = renderSystem->CreateBuffer(LLGL::ConstantBufferDesc(sizeof(Settings)),
	                                             &settings);
	LLGL::VertexFormat vertexFormat;
	vertexFormat.AppendAttribute({"position", LLGL::Format::RGB32Float});
	vertexFormat.AppendAttribute({"color", LLGL::Format::RGB32Float});

	std::string vertPath = SHADERS_FOLDER;
	vertPath.append("debug.vert");

	std::string fragPath = SHADERS_FOLDER;
	fragPath.append("debug.frag");

	mShader = std::make_unique<Shader>(*renderSystem, vertexFormat, vertPath.c_str(),
	                                   fragPath.c_str());

	// Create pipeline layout
	// TODO: Does the pipeline layout need to be removed if we aren't using resource heap?
	auto pipelineLayout = renderSystem->CreatePipelineLayout(
		LLGL::PipelineLayoutDesc("cbuffer(0):vert"));
	// Create graphics pipeline
	LLGL::GraphicsPipelineDescriptor pipelineDesc;
	{
		pipelineDesc.vertexShader = &mShader->GetVertexShader();
		pipelineDesc.fragmentShader = &mShader->GetFragmentShader();
		pipelineDesc.pipelineLayout = pipelineLayout;
		pipelineDesc.primitiveTopology = LLGL::PrimitiveTopology::LineList;
	}
	mPipeline = renderSystem->CreatePipelineState(pipelineDesc);

	mDebugDrawManager = DebugDrawManager::GetInstance();

	// pre-allocate the buffer with 500 vertices 
	mVertexBuffer = renderSystem->CreateBuffer(
		VertexBufferDesc(static_cast<std::uint32_t>(500 * sizeof(DebugVertex)),
		                 mShader->GetVertexFormat()));
}
