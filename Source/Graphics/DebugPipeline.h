#pragma once
#include <LLGL/LLGL.h>
#include <glm/mat4x4.hpp>

class RenderObject;
class Shader;
class Renderer;
class ResourceManager;

class DebugPipeline
{
public:
	DebugPipeline(Renderer& renderer, ResourceManager& resourceManager);
	~DebugPipeline() = default;

	void Render(LLGL::CommandBuffer& commands) const;

	void UpdateProjectionViewModelUniform(LLGL::CommandBuffer& commands, glm::mat4 model, glm::mat4 projection,
	                                      glm::mat4 view) const;

	void ClearDebugDraw() const;

	void AddRenderObjectVBuffer(RenderObject& obj) const;

private:
	void _InitPipeline();

	LLGL::PipelineState* mPipeline = nullptr;
	Shader* mShader = nullptr;

	Renderer& mRenderer;
	ResourceManager& mResourceManager;
};
