#pragma once
#include <LLGL/LLGL.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, perspective

#include "Graphics/Pipelines/ImmediatePipeline.h"
#include "Graphics/Pipelines/MapPipeline.h"
#include "Graphics/Pipelines/MeshPipeline.h"
#include "Graphics/Pipelines/SpritePipeline.h"
#include "Graphics/Pipelines/TextPipeline.h"

class Camera;
class EntityRegistry;
class InputHandler;
class MapRegistry;

class RenderContext
{
public:
	RenderContext(const LLGL::UTF8String& title, const LLGL::Extent2D screenSize,
	              const LLGL::ColorRGBAf backgroundColor,
	              const std::shared_ptr<InputHandler>& inputHandler);

	void BeginFrame() const;
	void Render(const std::unique_ptr<Camera>& camera, EntityRegistry& entityRegistry,
	            MapRegistry& mapRegistry) const;
	void EndFrame() const;
	bool ProcessEvents() const;

	void DrawText(const char* text, glm::vec2 position, glm::vec2 size);
	void DrawPoint(glm::vec3 pos, glm::vec3 color, float size) const;
	void DrawLine(glm::vec3 from, glm::vec3 to, glm::vec3 color) const;
	void DrawBox(glm::vec3 pos, glm::vec3 size, glm::vec3 color) const;
	void DrawCircle(glm::vec3 position, float radius, glm::vec3 color) const;
	void DrawGrid(glm::vec3 pos, glm::vec3 size, glm::vec3 color, int rows, int columns) const;

	void LoadFont(const char* fontFileName) const;
	bool GetNativeHandle(void* nativeHandle, std::size_t nativeHandleSize) const;
	// Color range from 0.0f - 1.0f
	void SetBackgroundClearColor(const LLGL::ColorRGBAf color);

	void UpdateProjection();
	glm::vec3 NormalizedDeviceCoords(glm::vec3 vec) const;
	glm::mat4 GetPerspectiveProjection() const;

	void ResizeBuffers(const LLGL::Extent2D& size) const;
private:
	void _CreateWindow(const LLGL::UTF8String& title, const std::shared_ptr<InputHandler>& inputHandler);
	void _CreatePipelines();

	static LLGL::Extent2D _ScaleResolution(const LLGL::Extent2D& res, float scale);

	std::shared_ptr<LLGL::RenderSystem> mRenderSystem; // Render system
	LLGL::SwapChain* mSwapChain = nullptr; // Main render context
	LLGL::CommandBuffer* mCommands = nullptr; // Main command buffer
	LLGL::CommandQueue* mCommandQueue = nullptr; // Command queue

	glm::mat4 mPerspectiveProjection = glm::identity<glm::mat4>();
	glm::mat4 mOrthoProjection = glm::identity<glm::mat4>();

	std::unique_ptr<SpritePipeline> mSpritePipeline;
	std::unique_ptr<MeshPipeline> mMeshPipeline;
	std::unique_ptr<MapPipeline> mMapPipeline;
	std::unique_ptr<ImmediatePipeline> mImmediatePipeline;
	std::unique_ptr<TextPipeline> mTextPipeline;

	LLGL::ColorRGBAf mBackgroundColor = {0.0f, 0.0f, 0.0f, 1.0f};
	std::uint32_t mSamplesCount = 1;
};

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
			mRenderContext.ResizeBuffers(clientAreaSize);
			mRenderContext.UpdateProjection();

			// Re-draw frame
			/*
			 *
			if (sender.isLoadingDone())
			{
				//mRenderer.Render();
			}
			 */
		}
	}

private:
	RenderContext& mRenderContext;
};
