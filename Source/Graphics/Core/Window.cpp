#include "RendererInstance.h"

#include "Window.h"

Window::Window() {
    init();
}

void Window::init()
{
    const auto renderer = RendererInstance::GetInstance();
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




