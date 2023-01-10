#pragma once

#include <LLGL/LLGL.h>
#include "Renderer.h"

class Window
{
public:
	Window(Renderer& renderer);
	~Window() = default;

	void init();

	LLGL::Window& GetWindow() const;

	bool isLoadingDone() const
	{
		return mIsLoadingDone;
	}

	void ShowCursor(bool show) const;
	bool IsCursorShowing() const;

private:
	bool mIsLoadingDone = false;
	Renderer& mRenderer;
};

class ResizeEventHandler : public LLGL::Window::EventListener
{
public:
	ResizeEventHandler(Renderer& ref, Window& window, LLGL::SwapChain* swapChain) : mRenderer(ref), mWindow(window),
		mSwapChain(swapChain)
	{
	}

	void OnResize(LLGL::Window& sender, const LLGL::Extent2D& clientAreaSize) override
	{
		if (clientAreaSize.width >= 4 && clientAreaSize.height >= 4)
		{
			mSwapChain->ResizeBuffers(clientAreaSize);

			// Re-draw frame
			if (mWindow.isLoadingDone())
			{
				mRenderer.UpdateProjection();
				mRenderer.OnDrawFrame([]()
				{
				});
			}
		}
	}

	void OnTimer(LLGL::Window& sender, std::uint32_t timerID) override
	{
		// Re-draw frame
		if (mWindow.isLoadingDone())
			mRenderer.OnDrawFrame([]()
			{
			});
	}

private:
	Renderer& mRenderer;
	Window& mWindow;
	LLGL::SwapChain* mSwapChain;
};
