#include <glm/ext/matrix_transform.hpp>
#include "LLGL/Misc/Utility.h"

#include "ImmediatePipeline.h"

void ImmediatePipeline::Render(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat)
{
	// If point size change is needed, confirm works  
	// glPointSize(5.0);
	commandBuffer.UpdateBuffer(*mVertexBuffer, 0, mFrameVertices.data(),
	                           static_cast<std::uint16_t>(mFrameVertices.size() * sizeof(
		                           DebugVertex)));


	// set graphics pipeline
	commandBuffer.SetPipelineState(*mPipeline);
	// TODO: Do we need a Resource heap to use constant buffer? Don't think so
	//commandBuffer.SetResourceHeap(*mResourceHeap);
	commandBuffer.SetResource(*mConstantBuffer, 0, LLGL::BindFlags::ConstantBuffer,
	                          LLGL::StageFlags::VertexStage | LLGL::StageFlags::FragmentStage);
	commandBuffer.SetVertexBuffer(*mVertexBuffer);

	UpdateProjectionViewUniform(commandBuffer, pvMat);

	commandBuffer.Draw(mFrameVertices.size(), 0);
}

void ImmediatePipeline::UpdateProjectionViewUniform(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat)
{
	settings.pvMat = pvMat;
	commandBuffer.UpdateBuffer(*mConstantBuffer, 0, &settings, sizeof(settings));
}

void ImmediatePipeline::Init(std::shared_ptr<LLGL::RenderSystem>& renderSystem)
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

	// pre-allocate the buffer with 500 vertices 
	mVertexBuffer = renderSystem->CreateBuffer(
		VertexBufferDesc(static_cast<std::uint32_t>(500 * sizeof(DebugVertex)),
		                 mShader->GetVertexFormat()));
}

glm::vec3 ImmediatePipeline::_CalculateModelPoint(glm::vec3 pos, glm::vec3 size, glm::vec3 basePoint)
{
	auto model = glm::mat4(1.0f);

	model = glm::translate(model, pos);
	model = glm::scale(model, size);

	return model * glm::vec4(basePoint, 1.0);
}

void ImmediatePipeline::DrawPoint(glm::vec3 pos, glm::vec3 color, float size)
{
	mFrameVertices.push_back({_CalculateModelPoint(pos, {size, size, 1}), color});
}

void ImmediatePipeline::DrawLine(glm::vec3 from, glm::vec3 to, glm::vec3 color)
{
	mFrameVertices.push_back({_CalculateModelPoint(from, {1.0f, 1.0f, 1.0f}), color});
	mFrameVertices.push_back({_CalculateModelPoint(to, {1.0f, 1.0f, 1.0f}), color});
}

void ImmediatePipeline::DrawBox(glm::vec3 pos, glm::vec3 size, glm::vec3 color)
{
	mFrameVertices.push_back({_CalculateModelPoint(pos, size, mBoxVertices[0]), color});
	mFrameVertices.push_back({_CalculateModelPoint(pos, size, mBoxVertices[1]), color});

	mFrameVertices.push_back({_CalculateModelPoint(pos, size, mBoxVertices[1]), color});
	mFrameVertices.push_back({_CalculateModelPoint(pos, size, mBoxVertices[3]), color});

	mFrameVertices.push_back({_CalculateModelPoint(pos, size, mBoxVertices[2]), color});
	mFrameVertices.push_back({_CalculateModelPoint(pos, size, mBoxVertices[3]), color});

	mFrameVertices.push_back({_CalculateModelPoint(pos, size, mBoxVertices[2]), color});
	mFrameVertices.push_back({_CalculateModelPoint(pos, size, mBoxVertices[0]), color});
}

void ImmediatePipeline::DrawGrid(glm::vec3 pos, glm::vec3 size, glm::vec3 color, int rows, int columns)
{
	const auto& p0 = mBoxVertices[0]; //a
	const auto& p1 = mBoxVertices[1]; //b
	const auto& p2 = mBoxVertices[2]; //c
	const auto& p3 = mBoxVertices[3]; //d

	mFrameVertices.push_back({_CalculateModelPoint(pos, size, p0), color});
	mFrameVertices.push_back({_CalculateModelPoint(pos, size, p1), color});

	mFrameVertices.push_back({_CalculateModelPoint(pos, size, p1), color});
	mFrameVertices.push_back({_CalculateModelPoint(pos, size, p3), color});

	mFrameVertices.push_back({_CalculateModelPoint(pos, size, p2), color});
	mFrameVertices.push_back({_CalculateModelPoint(pos, size, p3), color});

	mFrameVertices.push_back({_CalculateModelPoint(pos, size, p2), color});
	mFrameVertices.push_back({_CalculateModelPoint(pos, size, p0), color});

	const float totalYDist = abs(p1.y - p0.y);
	const float yAmountToJump = totalYDist / rows;
	for (int i = 1; i < rows; i++)
	{
		mFrameVertices.push_back({
			_CalculateModelPoint(pos, size, glm::vec3{p0.x, p0.y - (i * yAmountToJump), p0.z}),
			color
		});
		mFrameVertices.push_back({
			_CalculateModelPoint(pos, size, glm::vec3{p2.x, p2.y - (i * yAmountToJump), p2.z}),
			color
		});
	}

	const float totalXDist = abs(p2.x - p0.x);
	const float xAmountToJump = totalXDist / columns;
	for (int i = 1; i < columns; i++)
	{
		mFrameVertices.push_back({
			_CalculateModelPoint(pos, size, glm::vec3{p0.x + (i * xAmountToJump), p0.y, p0.z}),
			color
		});
		mFrameVertices.push_back({
			_CalculateModelPoint(pos, size, glm::vec3{p1.x + (i * xAmountToJump), p1.y, p1.z}),
			color
		});
	}
}
