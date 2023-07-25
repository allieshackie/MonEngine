#pragma once
#include <LLGL/LLGL.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, perspective

#include "Graphics/SpritePipeline.h"
#include "Graphics/GUIPipeline.h"
#include "Graphics/MapPipeline.h"
#include "Graphics/MeshPipeline.h"
#include "Graphics/Debug/DebugPipeline.h"

class EntityRegistry;
class GUIBase;
class GUISystem;
class LevelManager;
class MapEditor;
class MapSystem;
class ResourceManager;

struct State;

class Renderer
{
public:
	Renderer(EntityRegistry& entityRegistry, ResourceManager& resourceManager, std::string shadersFolderPath);

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
	void InitGUIPipeline(GUISystem& guiSystem, GUIBase& gui);

private:
	void _Init();

	std::unique_ptr<LLGL::RenderSystem> mRenderer; // Render system
	LLGL::SwapChain* mSwapChain = nullptr; // Main render context
	LLGL::CommandBuffer* mCommands = nullptr; // Main command buffer
	LLGL::CommandQueue* mCommandQueue = nullptr; // Command queue

	glm::mat4 mProjection = glm::identity<glm::mat4>();

	std::unique_ptr<SpritePipeline> mSpritePipeline;
	std::unique_ptr<PipelineGUI> mPipelineGUI;
	std::unique_ptr<MeshPipeline> mMeshPipeline;
	std::unique_ptr<MapPipeline> mMapPipeline;
	std::unique_ptr<DebugPipeline> mDebugPipeline;

	EntityRegistry& mEntityRegistry;
	ResourceManager& mResourceManager;

	std::string mShadersFolderPath;
};
