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
class LevelManager;
class MainGameGUI;
class MapEditor;
class MapSystem;
class ResourceManager;

struct State;

class Renderer
{
public:
	Renderer(EntityRegistry& entityRegistry, ResourceManager& resourceManager);

	virtual ~Renderer()
	{
		std::cout << "Delete Renderer" << std::endl;
	}

	void Render() const;

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

	glm::mat4 GetProjection() const;
	glm::vec3 NormalizedDeviceCoords(glm::vec3 vec) const;

	void InitPipelines(LevelManager& levelManager, MapSystem& mapSystem);
	void InitGUIPipeline(GUISystem& guiSystem, MainGameGUI& mainGameGUI);

private:
	void _Init();

	std::unique_ptr<LLGL::RenderSystem> mRenderer; // Render system
	LLGL::SwapChain* mSwapChain = nullptr; // Main render context
	LLGL::CommandBuffer* mCommands = nullptr; // Main command buffer
	LLGL::CommandQueue* mCommandQueue = nullptr; // Command queue

	glm::mat4 mProjection = glm::identity<glm::mat4>();

	std::unique_ptr<Pipeline2D> mPipeline2D;
	std::unique_ptr<PipelineGUI> mPipelineGUI;
	std::unique_ptr<Pipeline3D> mPipeline3D;
	std::unique_ptr<DebugPipeline> mDebugPipeline;

	EntityRegistry& mEntityRegistry;
	ResourceManager& mResourceManager;
};
