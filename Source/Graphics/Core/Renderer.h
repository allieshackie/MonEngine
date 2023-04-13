#pragma once
#include <LLGL/LLGL.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, perspective

#include "2DPipeline.h"
#include "3DPipeline.h"
#include "GUIPipeline.h"
#include "Debug/DebugPipeline.h"

class EntityRegistry;
class GUISystem;
class MainGameGUI;
class MapEditor;
class MapSystem;
class ResourceManager;

class Renderer
{
public:
	Renderer(ResourceManager& resourceManager, EntityRegistry& entityRegistry, MapSystem& mapSystem);

	virtual ~Renderer()
	{
		std::cout << "Delete Renderer" << std::endl;
	}

	void OnDrawFrame() const;

	LLGL::SwapChain& GetSwapChain() const
	{
		return *mSwapChain;
	}

	const std::unique_ptr<LLGL::RenderSystem>& GetRendererSystem() const
	{
		return mRenderer;
	}

	LLGL::CommandBuffer& GetCommandBuffer() const
	{
		return *mCommands;
	}

	void UpdateProjection();
	void UpdateView(glm::mat4 view);

	glm::mat4 GetProjection() const;
	glm::mat4 GetView() const;
	glm::vec3 NormalizedDeviceCoords(glm::vec3 vec) const;

	void InitGUIPipeline(GUISystem& guiSystem, MapEditor& mapEditor, MainGameGUI& mainGameGUI);

private:
	void _Init(MapSystem& mapSystem);

	std::unique_ptr<LLGL::RenderSystem> mRenderer; // Render system
	LLGL::SwapChain* mSwapChain = nullptr; // Main render context
	LLGL::CommandBuffer* mCommands = nullptr; // Main command buffer
	LLGL::CommandQueue* mCommandQueue = nullptr; // Command queue

	glm::mat4 mProjection = glm::identity<glm::mat4>();
	glm::mat4 mView = glm::identity<glm::mat4>();

	std::unique_ptr<Pipeline2D> mPipeline2D;
	std::unique_ptr<PipelineGUI> mPipelineGUI;
	std::unique_ptr<Pipeline3D> mPipeline3D;
	std::unique_ptr<DebugPipeline> mDebugPipeline;

	EntityRegistry& mEntityRegistry;
	ResourceManager& mResourceManager;
};
