#pragma once
#include <LLGL/LLGL.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, perspective

class InputHandler;

class RenderContext
{
public:
	RenderContext(const LLGL::Extent2D screenSize, const LLGL::ColorRGBAf backgroundColor, bool usePerspective,
	              const LLGL::UTF8String& title, const std::shared_ptr<InputHandler>& inputHandler,
	              bool transparent);
	~RenderContext();

	// Copy constructor/assignment operator
	RenderContext(const RenderContext& other) = delete;
	RenderContext& operator=(const RenderContext& other) = delete;
	// Move constructor/assignment operator
	RenderContext(RenderContext&& other) noexcept = default;
	RenderContext& operator=(RenderContext&& rhs) noexcept = default;

	void BeginFrame() const;
	void EndFrame() const;
	bool ProcessEvents() const;

	LLGL::CommandBuffer& GetCommands() const { return *mCommands; }
	bool GetSurfaceNativeHandle(void* nativeHandle, std::size_t nativeHandleSize) const;
	bool GetBackendNativeHandle(void* nativeHandle, std::size_t nativeHandleSize) const;
	bool GetCommandBufferNativeHandle(void* nativeHandle, std::size_t nativeHandleSize) const;
	const LLGL::RenderSystemPtr& GetRenderSystem() const { return mRenderSystem; }
	LLGL::Extent2D GetResolution() const;

	glm::mat4 GetProjection() const;
	void UpdateProjection();

	// Color range from 0.0f - 1.0f
	glm::vec3 NormalizedDeviceCoords(glm::vec3 vec) const;
	void ResizeBuffers(const LLGL::Extent2D& size) const;

	void SetBackgroundClearColor(const LLGL::ColorRGBAf color);

private:
	void _CreateWindow(const LLGL::UTF8String& title, const std::shared_ptr<InputHandler>& inputHandler,
	                   bool transparent);

	static LLGL::Extent2D _ScaleResolution(const LLGL::Extent2D& res, float scale);

	LLGL::RenderSystemPtr mRenderSystem; // Render system
	LLGL::SwapChain* mSwapChain = nullptr; // Main render context
	LLGL::CommandBuffer* mCommands = nullptr; // Main command buffer
	LLGL::CommandQueue* mCommandQueue = nullptr; // Command queue

	glm::mat4 mProjection = glm::identity<glm::mat4>();

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
		}
	}

private:
	RenderContext& mRenderContext;
};
