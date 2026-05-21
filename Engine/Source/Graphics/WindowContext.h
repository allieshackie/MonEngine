#pragma once
#include <LLGL/LLGL.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, perspective

class InputHandler;
class RenderSystem;

class WindowContext
{
public:
	WindowContext(RenderSystem& system, const LLGL::Extent2D screenSize, const LLGL::ColorRGBAf backgroundColor,
	              const LLGL::UTF8String& title, const std::shared_ptr<InputHandler>& inputHandler,
	              bool transparent);
	~WindowContext();

	// Copy constructor/assignment operator
	WindowContext(const WindowContext& other) = delete;
	WindowContext& operator=(const WindowContext& other) = delete;
	// Move constructor/assignment operator
	WindowContext(WindowContext&& other) noexcept = default;
	WindowContext& operator=(WindowContext&& rhs) noexcept = default;

	void BeginFrame(LLGL::CommandBuffer& commands) const;
	void EndFrame(LLGL::CommandBuffer& commands, LLGL::CommandQueue& queue) const;
	bool ProcessEvents() const;

	bool GetSurfaceNativeHandle(void* nativeHandle, std::size_t nativeHandleSize) const;
	LLGL::Extent2D GetResolution() const;

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


	RenderSystem& mSystem;
	LLGL::SwapChain* mSwapChain = nullptr; // Main render context

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
