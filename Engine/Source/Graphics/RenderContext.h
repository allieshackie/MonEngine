#pragma once
#include <LLGL/LLGL.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, perspective

#include "Graphics/Pipelines/ImmediatePipeline.h"
#include "Graphics/Pipelines/MapPipeline.h"
#include "Graphics/Pipelines/MeshPipeline.h"
#include "Graphics/Pipelines/TextPipeline.h"

struct EntityId;
class Camera;
class EntityRegistry;
class InputHandler;

class RenderContext
{
public:
	RenderContext(const LLGL::Extent2D screenSize, const LLGL::ColorRGBAf backgroundColor, bool usePerspective);

	~RenderContext();

	void InitPipelines(const LLGL::UTF8String& title, const std::shared_ptr<InputHandler>& inputHandler,
	                   EntityRegistry& entityRegistry, const ResourceManager& resourceManager);

	void BeginFrame() const;
	void Render(const Camera& camera, EntityRegistry& entityRegistry, ResourceManager& resourceManager) const;
	void EndFrame() const;
	bool ProcessEvents() const;

	void DrawText(const char* text, glm::vec2 position, glm::vec2 size, glm::vec4 color);
	void DrawPoint(glm::vec3 pos, glm::vec4 color, float size) const;
	void DrawLine(glm::vec3 from, glm::vec3 to, glm::vec4 color) const;
	void DrawBox(glm::vec3 pos, glm::vec3 size, glm::vec4 color, bool filled) const;
	void DrawCircle(glm::vec3 position, float radius, glm::vec4 color) const;
	void DrawGrid(glm::vec3 pos, glm::vec3 size, glm::vec4 color, int rows, int columns) const;

	void LoadFont(const char* fontFileName) const;
	bool GetSurfaceNativeHandle(void* nativeHandle, std::size_t nativeHandleSize) const;
	bool GetBackendNativeHandle(void* nativeHandle, std::size_t nativeHandleSize) const;
	bool GetCommandBufferNativeHandle(void* nativeHandle, std::size_t nativeHandleSize) const;
	// Color range from 0.0f - 1.0f
	void SetBackgroundClearColor(const LLGL::ColorRGBAf color);

	void UpdateProjection();
	glm::vec3 NormalizedDeviceCoords(glm::vec3 vec) const;
	glm::mat4 GetProjection() const;

	void ResizeBuffers(const LLGL::Extent2D& size) const;

	void GenerateMapTexture(EntityRegistry& entityRegistry, const ResourceManager& resourceManager,
	                        EntityId mapId) const;

	const LLGL::RenderSystemPtr& GetRenderSystem() const { return mRenderSystem; }

private:
	void _CreateWindow(const LLGL::UTF8String& title, const std::shared_ptr<InputHandler>& inputHandler);
	void _CreatePipelines(EntityRegistry& entityRegistry, const ResourceManager& resourceManager);

	static LLGL::Extent2D _ScaleResolution(const LLGL::Extent2D& res, float scale);

	LLGL::RenderSystemPtr mRenderSystem; // Render system
	LLGL::SwapChain* mSwapChain = nullptr; // Main render context
	LLGL::CommandBuffer* mCommands = nullptr; // Main command buffer
	LLGL::CommandQueue* mCommandQueue = nullptr; // Command queue

	glm::mat4 mProjection = glm::identity<glm::mat4>();

	std::unique_ptr<MeshPipeline> mMeshPipeline;
	std::unique_ptr<MapPipeline> mMapPipeline;
	std::unique_ptr<ImmediatePipeline> mImmediatePipeline;
	std::unique_ptr<TextPipeline> mTextPipeline;

	LLGL::ColorRGBAf mBackgroundColor = {0.0f, 0.0f, 0.0f, 1.0f};
	std::uint32_t mSamplesCount = 1;

	bool mUsePerspective = true;
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
