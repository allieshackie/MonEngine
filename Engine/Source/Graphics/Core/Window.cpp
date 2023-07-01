#include "Window.h"

Window::Window(Renderer& renderer)
	: mRenderer(renderer)
{
	init();
}

void Window::init()
{
	// get window from context surface
	auto& window = LLGL::CastTo<LLGL::Window>(mRenderer.GetSwapChain().GetSurface());

	// Needs L outside of quotes?
	window.SetTitle(L"Editor");

	// Change window descriptor to allow resizing
	auto wndDesc = window.GetDesc();
	wndDesc.resizable = true;
	window.SetDesc(wndDesc);

	// Change window behavior
	auto behavior = window.GetBehavior();
	behavior.disableClearOnResize = true;
	behavior.moveAndResizeTimerID = 1;
	window.SetBehavior(behavior);

	// Add window resize listener
	window.AddEventListener(std::make_shared<ResizeEventHandler>(mRenderer, *this, &mRenderer.GetSwapChain()));

	window.Show();

	mIsLoadingDone = true;
}

LLGL::Window& Window::GetWindow() const
{
	// get window from context surface
	return LLGL::CastTo<LLGL::Window>(mRenderer.GetSwapChain().GetSurface());
}

void Window::ShowCursor(bool show) const
{
	// get window from context surface
	const auto display = mRenderer.GetSwapChain().GetSurface().FindResidentDisplay();
	display->ShowCursor(show);
}

bool Window::IsCursorShowing() const
{
	// get window from context surface
	const auto display = mRenderer.GetSwapChain().GetSurface().FindResidentDisplay();
	return display->IsCursorShown();
}
