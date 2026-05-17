#pragma once
#include <LLGL/LLGL.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, perspective

class InputHandler;

class WindowContext
{
public:
	WindowContext(const LLGL::Extent2D screenSize, const LLGL::ColorRGBAf backgroundColor,
	              const LLGL::UTF8String& title, const std::shared_ptr<InputHandler>& inputHandler,
	              bool transparent);
	~WindowContext();

	// Copy constructor/assignment operator
	WindowContext(const WindowContext& other) = delete;
	WindowContext& operator=(const WindowContext& other) = delete;
	// Move constructor/assignment operator
	WindowContext(WindowContext&& other) noexcept = default;
	WindowContext& operator=(WindowContext&& rhs) noexcept = default;

	void BeginFrame() const;
	void EndFrame() const;
	bool ProcessEvents() const;

	LLGL::CommandBuffer& GetCommands() const { return *mCommands; }
	bool GetSurfaceNativeHandle(void* nativeHandle, std::size_t nativeHandleSize) const;
	bool GetBackendNativeHandle(void* nativeHandle, std::size_t nativeHandleSize) const;
	bool GetCommandBufferNativeHandle(void* nativeHandle, std::size_t nativeHandleSize) const;
	const LLGL::RenderSystemPtr& GetRenderSystem() const { return mRenderSystem; }
	LLGL::Extent2D GetResolution() const;

	glm::mat4 GetPerspectiveProjection() const { return mPerspectiveProjection; }
	glm::mat4 GetOrthoProjection() const { return mOrthoProjection; }
	void UpdateProjection();

	// Color range from 0.0f - 1.0f
	glm::vec3 NormalizedDeviceCoords(glm::vec3 vec) const;
	void ResizeBuffers(const LLGL::Extent2D& size);

	void SetBackgroundClearColor(const LLGL::ColorRGBAf color);

	bool HasViewportSizeChanged() { return mViewportSizeChanged; }
	void SetViewportSizeChanged(bool changed) { mViewportSizeChanged = changed; }

private:
	void _CreateWindow(const LLGL::UTF8String& title, const std::shared_ptr<InputHandler>& inputHandler,
	                   bool transparent);

	static LLGL::Extent2D _ScaleResolution(const LLGL::Extent2D& res, float scale);

	LLGL::RenderSystemPtr mRenderSystem; // Render system
	LLGL::SwapChain* mSwapChain = nullptr; // Main render context
	LLGL::CommandBuffer* mCommands = nullptr; // Main command buffer
	LLGL::CommandQueue* mCommandQueue = nullptr; // Command queue

	glm::mat4 mPerspectiveProjection = glm::identity<glm::mat4>();
	glm::mat4 mOrthoProjection = glm::identity<glm::mat4>();

	LLGL::ColorRGBAf mBackgroundColor = {0.0f, 0.0f, 0.0f, 1.0f};
	std::uint32_t mSamplesCount = 1;

	bool mViewportSizeChanged = false;
};

class ResizeEventHandler : public LLGL::Window::EventListener
{
public:
	ResizeEventHandler(WindowContext& cxt) : mWindowContext(cxt)
	{
	}

	void OnResize(LLGL::Window& sender, const LLGL::Extent2D& clientAreaSize) override
	{
		if (clientAreaSize.width >= 4 && clientAreaSize.height >= 4)
		{
			// Update swap buffers
			mWindowContext.ResizeBuffers(clientAreaSize);
			mWindowContext.UpdateProjection();
		}
	}

private:
	WindowContext& mWindowContext;
};
