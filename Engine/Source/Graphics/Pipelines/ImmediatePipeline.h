#pragma once
#include <array>
#include <LLGL/LLGL.h>
#include <glm/mat4x4.hpp>

#include "Graphics/Core/Shader.h"
#include "Graphics/Core/Vertex.h"

class ImmediatePipeline
{
public:
	void Init(LLGL::RenderSystemPtr& renderSystem);
	void Render(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat);

	void Release(const LLGL::RenderSystemPtr& renderSystem);

	void UpdateProjectionViewUniform(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat);

	// Draw Helpers
	void DrawPoint(glm::vec3 pos, glm::vec4 color, float size);
	void DrawLine(glm::vec3 from, glm::vec3 to, glm::vec4 color);
	void DrawBox(glm::vec3 pos, glm::vec3 size, glm::vec4 color, bool filled);
	void DrawCircle(glm::vec3 position, float radius, glm::vec4 color);
	void DrawGrid(glm::vec3 pos, glm::vec3 size, glm::vec4 color, int rows, int columns);

private:
	static glm::vec3 _CalculateModelPoint(glm::vec3 pos, glm::vec3 size, glm::vec3 basePoint = {0, 0, 0});

	void _RenderPoints(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat);
	void _RenderLines(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat);
	void _RenderCircles(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat);

	void _DrawFilledBox(glm::vec3 pos, glm::vec3 size, glm::vec4 color);
	void _DrawOutlinedBox(glm::vec3 pos, glm::vec3 size, glm::vec4 color);

	void _ClearVertices();

	void _InitResourceHeap(const LLGL::RenderSystemPtr& renderSystem);

	struct Settings
	{
		glm::mat4 pvMat;
	}
	settings = {};

	const std::array<glm::vec3, 4> mBoxVertices{{{0, 1.0, 1.0}, {0, 0, 1.0}, {1.0, 1.0, 1.0}, {1.0, 0, 1.0}}};

	LLGL::PipelineLayout* mPipelineLayout = nullptr;

	LLGL::PipelineState* mPointPipeline = nullptr;
	LLGL::PipelineState* mLinePipeline = nullptr;
	LLGL::PipelineState* mCirclePipeline = nullptr;

	LLGL::Buffer* mConstantBuffer = nullptr;

	LLGL::Buffer* mPointVertexBuffer = nullptr;
	LLGL::Buffer* mLineVertexBuffer = nullptr;
	LLGL::Buffer* mCircleVertexBuffer = nullptr;

	std::unique_ptr<Shader> mShader;

	std::vector<DebugVertex> mFramePointVertices;
	std::vector<DebugVertex> mFrameLineVertices;
	std::vector<DebugVertex> mFrameCircleVertices;

	LLGL::ResourceHeap* mResourceHeap = nullptr;
};
