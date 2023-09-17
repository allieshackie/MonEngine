#pragma once
#include <LLGL/LLGL.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, perspective

#include "Graphics/Pipelines/DebugPipeline.h"
#include "Graphics/Pipelines/MapPipeline.h"
#include "Graphics/Pipelines/MeshPipeline.h"
#include "Graphics/Pipelines/SpritePipeline.h"
#include "Graphics/Pipelines/TextPipeline.h"

class Camera;
class EntityRegistry;
class GUIBase;
class GUISystem;
class InputHandler;
class LevelManager;
class MapEditor;
class MapSystem;

struct State;

class RenderContext
{
public:
	virtual ~RenderContext()
	{
		std::cout << "Delete Renderer" << std::endl;
	}

	void Init(std::shared_ptr<InputHandler>& inputHandler);

	void BeginFrame() const;
	void Render(const std::unique_ptr<Camera>& camera, EntityRegistry& entityRegistry) const;
	void EndFrame() const;
	bool ProcessEvents() const;

	void DrawText(const char* text, glm::vec2 position, glm::vec2 size);

	void LoadFont(const char* fontFileName) const;
	bool GetNativeHandle(void* nativeHandle, std::size_t nativeHandleSize) const;

	void UpdateProjection();
	glm::vec3 NormalizedDeviceCoords(glm::vec3 vec) const;
	glm::mat4 GetPerspectiveProjection() const;
private:
	void _InitWindow(std::shared_ptr<InputHandler>& inputHandler) const;

	std::shared_ptr<LLGL::RenderSystem> mRenderSystem; // Render system
	LLGL::SwapChain* mSwapChain = nullptr; // Main render context
	LLGL::CommandBuffer* mCommands = nullptr; // Main command buffer
	LLGL::CommandQueue* mCommandQueue = nullptr; // Command queue

	glm::mat4 mPerspectiveProjection = glm::identity<glm::mat4>();
	glm::mat4 mOrthoProjection = glm::identity<glm::mat4>();

	std::unique_ptr<SpritePipeline> mSpritePipeline;
	std::unique_ptr<MeshPipeline> mMeshPipeline;
	std::unique_ptr<MapPipeline> mMapPipeline;
	std::unique_ptr<DebugPipeline> mDebugPipeline;
	std::unique_ptr<TextPipeline> mTextPipeline;
};

/* TODO: Re-implement resize
 *
class ResizeEventHandler : public LLGL::Window::EventListener
{
public:
	ResizeEventHandler(RenderContext& cxt) : mRenderContext(cxt)
	{
	}

	void OnResize(LLGL::Window& sender, const LLGL::Extent2D& clientAreaSize) override
	{
		if (clientAreaSize.width >= 4 && clientAreaSize.height >= 4)
		{
			// Update swap buffers
			mSwapChain->ResizeBuffers(clientAreaSize);

			// Re-draw frame
			if (mWindow.isLoadingDone())
			{
				mRenderer.UpdateProjection();
				mRenderer.Render();
			}
		}
	}

	void OnTimer(LLGL::Window& sender, std::uint32_t timerID) override
	{
		// Re-draw frame
		if (mWindow.isLoadingDone())
			mRenderer.Render();
	}

private:
	RenderContext& mRenderContext;
};
 */
