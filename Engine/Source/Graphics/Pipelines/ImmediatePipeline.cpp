#include <glm/ext/matrix_transform.hpp>
#include "LLGL/Utils/Parse.h"
#include "LLGL/Utils/Utility.h"

#include "ImmediatePipeline.h"

void ImmediatePipeline::Render(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat)
{
	_RenderPoints(commandBuffer, pvMat);
	_RenderLines(commandBuffer, pvMat);
	_RenderCircles(commandBuffer, pvMat);

	_ClearVertices();
}

void ImmediatePipeline::Release(const LLGL::RenderSystemPtr& renderSystem)
{
	renderSystem->Release(*mPointPipeline);
	mPointPipeline = nullptr;

	renderSystem->Release(*mLinePipeline);
	mLinePipeline = nullptr;

	renderSystem->Release(*mCirclePipeline);
	mCirclePipeline = nullptr;

	renderSystem->Release(*mConstantBuffer);
	mConstantBuffer = nullptr;

	renderSystem->Release(*mPointVertexBuffer);
	mPointVertexBuffer = nullptr;

	renderSystem->Release(*mLineVertexBuffer);
	mLineVertexBuffer = nullptr;

	renderSystem->Release(*mCircleVertexBuffer);
	mCircleVertexBuffer = nullptr;
}

void ImmediatePipeline::UpdateProjectionViewUniform(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat)
{
	settings.pvMat = pvMat;
	commandBuffer.UpdateBuffer(*mConstantBuffer, 0, &settings, sizeof(settings));
}

void ImmediatePipeline::Init(LLGL::RenderSystemPtr& renderSystem)
{
	mConstantBuffer = renderSystem->CreateBuffer(LLGL::ConstantBufferDesc(sizeof(Settings)),
	                                             &settings);
	LLGL::VertexFormat vertexFormat;
	vertexFormat.AppendAttribute({"position", LLGL::Format::RGB32Float});
	vertexFormat.AppendAttribute({"color", LLGL::Format::RGBA32Float});

	std::string vertPath = SHADERS_FOLDER;
	vertPath.append("debug.vert");

	std::string fragPath = SHADERS_FOLDER;
	fragPath.append("debug.frag");

	mShader = std::make_unique<Shader>(*renderSystem, vertexFormat, vertPath.c_str(),
	                                   fragPath.c_str());

	// Create pipeline layout
	// TODO: Does the pipeline layout need to be removed if we aren't using resource heap?
	mPipelineLayout = renderSystem->CreatePipelineLayout(
		LLGL::Parse("heap{cbuffer(0):vert}"));
	// Create graphics pipeline
	LLGL::GraphicsPipelineDescriptor pointPipelineDesc;
	{
		pointPipelineDesc.vertexShader = &mShader->GetVertexShader();
		pointPipelineDesc.fragmentShader = &mShader->GetFragmentShader();
		pointPipelineDesc.pipelineLayout = mPipelineLayout;
		pointPipelineDesc.primitiveTopology = LLGL::PrimitiveTopology::PointList;
		pointPipelineDesc.blend.targets[0].blendEnabled = true;
	}
	mPointPipeline = renderSystem->CreatePipelineState(pointPipelineDesc);
	// pre-allocate the buffer with 500 vertices 
	mPointVertexBuffer = renderSystem->CreateBuffer(
		VertexBufferDesc(static_cast<std::uint32_t>(200 * sizeof(DebugVertex)),
		                 mShader->GetVertexFormat()));


	// Create graphics pipeline
	LLGL::GraphicsPipelineDescriptor linePipelineDesc;
	{
		linePipelineDesc.vertexShader = &mShader->GetVertexShader();
		linePipelineDesc.fragmentShader = &mShader->GetFragmentShader();
		linePipelineDesc.pipelineLayout = mPipelineLayout;
		linePipelineDesc.primitiveTopology = LLGL::PrimitiveTopology::LineList;
		linePipelineDesc.blend.targets[0].blendEnabled = true;
	}
	mLinePipeline = renderSystem->CreatePipelineState(linePipelineDesc);
	// pre-allocate the buffer with 500 vertices 
	mLineVertexBuffer = renderSystem->CreateBuffer(
		VertexBufferDesc(static_cast<std::uint32_t>(200 * sizeof(DebugVertex)),
		                 mShader->GetVertexFormat()));


	// Create graphics pipeline
	LLGL::GraphicsPipelineDescriptor circlePipelineDesc;
	{
		circlePipelineDesc.vertexShader = &mShader->GetVertexShader();
		circlePipelineDesc.fragmentShader = &mShader->GetFragmentShader();
		circlePipelineDesc.pipelineLayout = mPipelineLayout;
		circlePipelineDesc.primitiveTopology = LLGL::PrimitiveTopology::TriangleList;
		circlePipelineDesc.blend.targets[0].blendEnabled = true;
	}
	mCirclePipeline = renderSystem->CreatePipelineState(circlePipelineDesc);
	// pre-allocate the buffer with 500 vertices 
	mCircleVertexBuffer = renderSystem->CreateBuffer(
		VertexBufferDesc(static_cast<std::uint32_t>(1000 * sizeof(DebugVertex)),
		                 mShader->GetVertexFormat()));

	_InitResourceHeap(renderSystem);
}

void ImmediatePipeline::_RenderPoints(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat)
{
	// If point size change is needed, confirm works  
	// glPointSize(5.0);
	commandBuffer.UpdateBuffer(*mPointVertexBuffer, 0, mFramePointVertices.data(),
	                           static_cast<std::uint16_t>(mFramePointVertices.size() * sizeof(
		                           DebugVertex)));

	// set graphics pipeline
	commandBuffer.SetPipelineState(*mPointPipeline);
	commandBuffer.SetResourceHeap(*mResourceHeap);
	commandBuffer.SetVertexBuffer(*mPointVertexBuffer);
	UpdateProjectionViewUniform(commandBuffer, pvMat);
	commandBuffer.Draw(mFramePointVertices.size(), 0);
}

void ImmediatePipeline::_RenderLines(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat)
{
	// If point size change is needed, confirm works  
	// glPointSize(5.0);
	commandBuffer.UpdateBuffer(*mLineVertexBuffer, 0, mFrameLineVertices.data(),
	                           static_cast<std::uint16_t>(mFrameLineVertices.size() * sizeof(
		                           DebugVertex)));

	// set graphics pipeline
	commandBuffer.SetPipelineState(*mLinePipeline);
	commandBuffer.SetResourceHeap(*mResourceHeap);
	commandBuffer.SetVertexBuffer(*mLineVertexBuffer);
	UpdateProjectionViewUniform(commandBuffer, pvMat);
	commandBuffer.Draw(mFrameLineVertices.size(), 0);
}

void ImmediatePipeline::_RenderCircles(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat)
{
	// If point size change is needed, confirm works  
	// glPointSize(5.0);
	commandBuffer.UpdateBuffer(*mCircleVertexBuffer, 0, mFrameCircleVertices.data(),
	                           static_cast<std::uint16_t>(mFrameCircleVertices.size() * sizeof(
		                           DebugVertex)));

	// set graphics pipeline
	commandBuffer.SetPipelineState(*mCirclePipeline);
	commandBuffer.SetResourceHeap(*mResourceHeap);
	commandBuffer.SetVertexBuffer(*mCircleVertexBuffer);
	UpdateProjectionViewUniform(commandBuffer, pvMat);
	commandBuffer.Draw(mFrameCircleVertices.size(), 0);
}

void ImmediatePipeline::_ClearVertices()
{
	mFramePointVertices.clear();
	mFrameLineVertices.clear();
	mFrameCircleVertices.clear();
}

void ImmediatePipeline::_InitResourceHeap(const LLGL::RenderSystemPtr& renderSystem)
{
	// Resource Heap

	std::vector<LLGL::ResourceViewDescriptor> resourceViews;
	resourceViews.emplace_back(mConstantBuffer);

	mResourceHeap = renderSystem->CreateResourceHeap(mPipelineLayout, resourceViews);
}

glm::vec3 ImmediatePipeline::_CalculateModelPoint(glm::vec3 pos, glm::vec3 size, glm::vec3 basePoint)
{
	auto model = glm::mat4(1.0f);

	model = glm::translate(model, pos);
	model = glm::scale(model, size);

	return model * glm::vec4(basePoint, 1.0);
}

void ImmediatePipeline::DrawPoint(glm::vec3 pos, glm::vec4 color, float size)
{
	mFramePointVertices.push_back({_CalculateModelPoint(pos, {size, size, 1}), color});
}

void ImmediatePipeline::DrawLine(glm::vec3 from, glm::vec3 to, glm::vec4 color)
{
	mFrameLineVertices.push_back({_CalculateModelPoint(from, {1.0f, 1.0f, 1.0f}), color});
	mFrameLineVertices.push_back({_CalculateModelPoint(to, {1.0f, 1.0f, 1.0f}), color});
}

void ImmediatePipeline::DrawBox(glm::vec3 pos, glm::vec3 size, glm::vec4 color, bool filled)
{
	if (filled)
	{
		_DrawFilledBox(pos, size, color);
	}
	else
	{
		_DrawOutlinedBox(pos, size, color);
	}
}

void ImmediatePipeline::DrawCircle(glm::vec3 position, float radius, glm::vec4 color)
{
	/* TODO: Hollowed circle
	 *
	for (int i = 0; i < 20; ++i)
	{
		const float angle = 6.28f * i / 20;
		const float x = position.x + radius * cos(angle);
		const float y = position.y + radius * sin(angle);

		mFrameCircleVertices.push_back({
			_CalculateModelPoint({position.x + x, position.y - y, position.z}, {1.0f, 1.0f, 1.0f}), color
		});
	}
	 */

	for (int i = 0; i < 19; ++i)
	{
		const float angle1 = 6.28f * i / 19;
		const float angle2 = 6.28f * (i + 1) / 19;
		const float x1 = position.x + radius * cos(angle1);
		const float y1 = position.y + radius * sin(angle1);
		const float x2 = position.x + radius * cos(angle2);
		const float y2 = position.y + radius * sin(angle2);

		// Add the center vertex
		mFrameCircleVertices.push_back({
			_CalculateModelPoint(position, {1.0f, 1.0f, 1.0f}), color
		});

		// Add the first vertex on the edge of the circle
		mFrameCircleVertices.push_back({
			_CalculateModelPoint({x1, y1, position.z}, {1.0f, 1.0f, 1.0f}), color
		});

		// Add the second vertex on the edge of the circle
		mFrameCircleVertices.push_back({
			_CalculateModelPoint({x2, y2, position.z}, {1.0f, 1.0f, 1.0f}), color
		});
	}

	// Add the final triangle to close the circle
	const float angle1 = 0.0f;
	const float angle2 = 6.28f;
	const float x1 = position.x + radius * cos(angle1);
	const float y1 = position.y + radius * sin(angle1);
	const float x2 = position.x + radius * cos(angle2);
	const float y2 = position.y + radius * sin(angle2);

	// Add the center vertex
	mFrameCircleVertices.push_back({
		_CalculateModelPoint(position, {1.0f, 1.0f, 1.0f}), color
	});

	// Add the first vertex on the edge of the circle
	mFrameCircleVertices.push_back({
		_CalculateModelPoint({x1, y1, position.z}, {1.0f, 1.0f, 1.0f}), color
	});

	// Add the second vertex on the edge of the circle
	mFrameCircleVertices.push_back({
		_CalculateModelPoint({x2, y2, position.z}, {1.0f, 1.0f, 1.0f}), color
	});
}

void ImmediatePipeline::DrawGrid(glm::vec3 pos, glm::vec3 size, glm::vec4 color, int rows, int columns)
{
	const auto& p0 = mBoxVertices[0]; //a
	const auto& p1 = mBoxVertices[1]; //b
	const auto& p2 = mBoxVertices[2]; //c
	const auto& p3 = mBoxVertices[3]; //d

	mFrameLineVertices.push_back({_CalculateModelPoint(pos, size, p0), color});
	mFrameLineVertices.push_back({_CalculateModelPoint(pos, size, p1), color});

	mFrameLineVertices.push_back({_CalculateModelPoint(pos, size, p1), color});
	mFrameLineVertices.push_back({_CalculateModelPoint(pos, size, p3), color});

	mFrameLineVertices.push_back({_CalculateModelPoint(pos, size, p2), color});
	mFrameLineVertices.push_back({_CalculateModelPoint(pos, size, p3), color});

	mFrameLineVertices.push_back({_CalculateModelPoint(pos, size, p2), color});
	mFrameLineVertices.push_back({_CalculateModelPoint(pos, size, p0), color});

	const float totalYDist = abs(p1.y - p0.y);
	const float yAmountToJump = totalYDist / rows;
	for (int i = 1; i < rows; i++)
	{
		mFrameLineVertices.push_back({
			_CalculateModelPoint(pos, size, glm::vec3{p0.x, p0.y - (i * yAmountToJump), p0.z}),
			color
		});
		mFrameLineVertices.push_back({
			_CalculateModelPoint(pos, size, glm::vec3{p2.x, p2.y - (i * yAmountToJump), p2.z}),
			color
		});
	}

	const float totalXDist = abs(p2.x - p0.x);
	const float xAmountToJump = totalXDist / columns;
	for (int i = 1; i < columns; i++)
	{
		mFrameLineVertices.push_back({
			_CalculateModelPoint(pos, size, glm::vec3{p0.x + (i * xAmountToJump), p0.y, p0.z}),
			color
		});
		mFrameLineVertices.push_back({
			_CalculateModelPoint(pos, size, glm::vec3{p1.x + (i * xAmountToJump), p1.y, p1.z}),
			color
		});
	}
}

void ImmediatePipeline::_DrawFilledBox(glm::vec3 pos, glm::vec3 size, glm::vec4 color)
{
	mFrameCircleVertices.push_back({_CalculateModelPoint(pos, size, mBoxVertices[0]), color});
	mFrameCircleVertices.push_back({_CalculateModelPoint(pos, size, mBoxVertices[1]), color});
	mFrameCircleVertices.push_back({_CalculateModelPoint(pos, size, mBoxVertices[3]), color});

	mFrameCircleVertices.push_back({_CalculateModelPoint(pos, size, mBoxVertices[2]), color});
	mFrameCircleVertices.push_back({_CalculateModelPoint(pos, size, mBoxVertices[3]), color});
	mFrameCircleVertices.push_back({_CalculateModelPoint(pos, size, mBoxVertices[0]), color});
}

void ImmediatePipeline::_DrawOutlinedBox(glm::vec3 pos, glm::vec3 size, glm::vec4 color)
{
	mFrameLineVertices.push_back({_CalculateModelPoint(pos, size, mBoxVertices[0]), color});
	mFrameLineVertices.push_back({_CalculateModelPoint(pos, size, mBoxVertices[1]), color});

	mFrameLineVertices.push_back({_CalculateModelPoint(pos, size, mBoxVertices[1]), color});
	mFrameLineVertices.push_back({_CalculateModelPoint(pos, size, mBoxVertices[3]), color});

	mFrameLineVertices.push_back({_CalculateModelPoint(pos, size, mBoxVertices[2]), color});
	mFrameLineVertices.push_back({_CalculateModelPoint(pos, size, mBoxVertices[3]), color});

	mFrameLineVertices.push_back({_CalculateModelPoint(pos, size, mBoxVertices[2]), color});
	mFrameLineVertices.push_back({_CalculateModelPoint(pos, size, mBoxVertices[0]), color});
}
