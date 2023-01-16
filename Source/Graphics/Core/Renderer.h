#pragma once
#include <LLGL/LLGL.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, perspective
#include "2DPipeline.h"
#include "3DPipeline.h"
#include "DebugPipeline.h"

class Box;
class DebugDrawable;
class Line;
class ResourceManager;
class Shader;

class RenderObject;

class Renderer
{
public:
	Renderer(ResourceManager& resourceManager);
	~Renderer();

	void OnDrawFrame(const std::function<void()>& drawCallback) const;

	LLGL::SwapChain& GetSwapChain() const
	{
		return *mSwapChain;
	}

	const std::unique_ptr<LLGL::RenderSystem>& GetRendererSystem() const
	{
		return mRenderer;
	}

	void UpdateProjection();
	void UpdateView(glm::mat4 view);

	glm::mat4 GetProjection() const;
	glm::vec3 NormalizedDeviceCoords(glm::vec3 vec) const;

	// 2D Pipeline
	void Update2DProjectionViewModelUniform(glm::mat4 model) const;
	void Update2DTextureClipUniform(glm::mat4 textureClip) const;
	void Add2DRenderObject(RenderObject& obj) const;

	// 3D Pipeline
	void Update3DProjectionViewModelUniform(glm::mat4 model) const;
	void Add3DRenderObject(RenderObject& obj) const;

	// Debug Pipeline
	void UpdateDebugProjectionViewModelUniform(glm::mat4 model) const;
	void ClearDebugDraw() const;
	void AddDebugRenderObject(RenderObject& obj) const;

private:
	void _Init();

	std::unique_ptr<LLGL::RenderSystem> mRenderer; // Render system
	LLGL::SwapChain* mSwapChain = nullptr; // Main render context
	LLGL::CommandBuffer* mCommands = nullptr; // Main command buffer
	LLGL::CommandQueue* mCommandQueue = nullptr; // Command queue

	glm::mat4 mProjection = glm::identity<glm::mat4>();
	glm::mat4 mView = glm::identity<glm::mat4>();

	std::unique_ptr<Pipeline2D> mPipeline2D;
	std::unique_ptr<Pipeline3D> mPipeline3D;
	std::unique_ptr<DebugPipeline> mDebugPipeline;

	ResourceManager& mResourceManager;
};
