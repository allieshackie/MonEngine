#pragma once
#include <LLGL/LLGL.h>
#include <glm/mat4x4.hpp>
#include "Graphics/Core/Shader.h"

class DebugDrawManager;

class DebugPipeline
{
public:
	DebugPipeline() = default;

	~DebugPipeline()
	{
		std::cout << "Delete DebugPipeline" << std::endl;
	}

	void Init(std::shared_ptr<LLGL::RenderSystem>& renderSystem);
	void Render(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat);

	void UpdateProjectionViewUniform(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat);

private:
	struct Settings
	{
		glm::mat4 pvMat;
	}
	settings = {};

	LLGL::PipelineState* mPipeline = nullptr;

	LLGL::Buffer* mConstantBuffer = nullptr;
	LLGL::Buffer* mVertexBuffer = nullptr;

	std::unique_ptr<Shader> mShader;
	DebugDrawManager* mDebugDrawManager = nullptr;
};
