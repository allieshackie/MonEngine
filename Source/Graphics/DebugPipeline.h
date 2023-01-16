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
	~DebugPipeline();

	void Render(LLGL::CommandBuffer& commands) const;

	void UpdateProjectionViewModelUniform(LLGL::CommandBuffer& commands, glm::mat4 model, glm::mat4 projection,
	                                      glm::mat4 view);

	void ClearDebugDraw() const;

	void AddRenderObjectVBuffer(RenderObject& obj) const;

private:
	void _InitPipeline();

	struct Settings
	{
		glm::mat4 pvmMat;
	}
	settings = {};

	LLGL::PipelineState* mPipeline = nullptr;
	LLGL::ResourceHeap* mResourceHeap = nullptr;
	LLGL::Buffer* mConstantBuffer = nullptr;
	std::unique_ptr<Shader> mShader = nullptr;

	Renderer& mRenderer;
	ResourceManager& mResourceManager;
};
