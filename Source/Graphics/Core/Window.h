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
	ResizeEventHandler(Renderer& ref, Window& window, LLGL::RenderContext* context) : mRenderer(ref), mWindow(window),
		mContext(context)
	{
	}

	void OnResize(LLGL::Window& sender, const LLGL::Extent2D& clientAreaSize) override
	{
		if (clientAreaSize.width >= 4 && clientAreaSize.height >= 4)
		{
			// Update video mode
			auto videoMode = mContext->GetVideoMode();
			{
				videoMode.resolution = clientAreaSize;
			}
			mContext->SetVideoMode(videoMode);

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
	LLGL::RenderContext* mContext;
};
