#pragma once
#include <array>
#include <LLGL/LLGL.h>
#include <glm/mat4x4.hpp>

#include "Graphics/Core/Shader.h"
#include "Graphics/Core/Vertex.h"

class ImmediatePipeline
{
public:
	void Init(std::shared_ptr<LLGL::RenderSystem>& renderSystem);
	void Render(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat);

	void UpdateProjectionViewUniform(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat);

	// Draw Helpers
	void DrawPoint(glm::vec3 pos, glm::vec3 color, float size);
	void DrawLine(glm::vec3 from, glm::vec3 to, glm::vec3 color);
	void DrawBox(glm::vec3 pos, glm::vec3 size, glm::vec3 color);
	void DrawGrid(glm::vec3 pos, glm::vec3 size, glm::vec3 color, int rows, int columns);

private:
	static glm::vec3 _CalculateModelPoint(glm::vec3 pos, glm::vec3 size, glm::vec3 basePoint = {0, 0, 0});

	struct Settings
	{
		glm::mat4 pvMat;
	}
	settings = {};

	const std::array<glm::vec3, 4> mBoxVertices{{{-0.5, 0.5, 1}, {-0.5, -0.5, 1}, {0.5, 0.5, 1}, {0.5, -0.5, 1}}};

	LLGL::PipelineState* mPipeline = nullptr;

	LLGL::Buffer* mConstantBuffer = nullptr;
	LLGL::Buffer* mVertexBuffer = nullptr;

	std::unique_ptr<Shader> mShader;
	std::vector<DebugVertex> mFrameVertices;
};
