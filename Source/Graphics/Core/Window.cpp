#include "Window.h"

Window::Window() {
    init();
}

void Window::init()
{
    const auto renderer = Renderer::GetInstance();
    // get window from context surface
    auto& window = LLGL::CastTo<LLGL::Window>(renderer->GetContext().GetSurface());

    // Needs L outside of quotes?
    window.SetTitle(L"Editor");

    // Change window descriptor to allow resizing
    auto wndDesc = window.GetDesc();
    wndDesc.resizable = true;
    window.SetDesc(wndDesc);

    // Change window behavior
    auto behavior = window.GetBehavior();
    {
        behavior.disableClearOnResize = true;
        behavior.moveAndResizeTimerID = 1;
    }
    window.SetBehavior(behavior);

    // Add window resize listener
    window.AddEventListener(std::make_shared<ResizeEventHandler>(*renderer, *this, &renderer->GetContext()));

    window.Show();

    mIsLoadingDone = true;
}

LLGL::Window& Window::GetWindow()
{
    // get window from context surface
    return LLGL::CastTo<LLGL::Window>(Renderer::GetInstance()->GetContext().GetSurface());
}

void Window::ShowCursor(bool show)
{
    // get window from context surface
    const auto display = Renderer::GetInstance()->GetContext().GetSurface().FindResidentDisplay();
    display->ShowCursor(show);
}

bool Window::IsCursorShowing()
{
    // get window from context surface
    const auto display = Renderer::GetInstance()->GetContext().GetSurface().FindResidentDisplay();
    return display->IsCursorShown();
}




