#include "LLGL/Platform/Win32/Win32NativeHandle.h"
#include "LLGL/Utils/TypeNames.h"
#include <glm/ext/matrix_clip_space.hpp>

#include "Input/InputHandler.h"

#include "RenderContext.h"

RenderContext::RenderContext(const LLGL::Extent2D screenSize, const LLGL::ColorRGBAf backgroundColor,
                             bool usePerspective, const LLGL::UTF8String& title,
                             const std::shared_ptr<InputHandler>& inputHandler,
                             bool transparent)
	: mBackgroundColor(backgroundColor), mUsePerspective(usePerspective)
{
	try
	{
		LLGL::Report report;
		// Load render system module (hard code to OpenGL for now)
		mRenderSystem = LLGL::RenderSystem::Load("OpenGL", &report);
		// Set report callback to standard output
		LLGL::Log::RegisterCallbackStd();

		const auto display = LLGL::Display::GetPrimary();

		// Create swap-chain
		LLGL::SwapChainDescriptor swapChainDesc;
		{
			swapChainDesc.resolution = display != nullptr
				                           ? _ScaleResolution(screenSize, display->GetScale())
				                           : screenSize;
			swapChainDesc.samples = mSamplesCount;
		}
		mSwapChain = mRenderSystem->CreateSwapChain(swapChainDesc);

		mSwapChain->SetVsyncInterval(1);
		mSwapChain->SetName("SwapChain");

		LLGL::CommandBufferDescriptor cmdBufferDesc;
		{
			cmdBufferDesc.flags = LLGL::CommandBufferFlags::ImmediateSubmit;
		}
		// Create command buffer to submit graphics commands
		mCommands = mRenderSystem->CreateCommandBuffer(cmdBufferDesc);

		// Get command queue to record and submit command buffers
		mCommandQueue = mRenderSystem->GetCommandQueue();

		// Print renderer information
		const auto& info = mRenderSystem->GetRendererInfo();
		const auto swapChainRes = mSwapChain->GetResolution();

		std::cout << "render system:" << std::endl;
		std::cout << "  renderer:           " << info.rendererName << std::endl;
		std::cout << "  device:             " << info.deviceName << std::endl;
		std::cout << "  vendor:             " << info.vendorName << std::endl;
		std::cout << "  shading language:   " << info.shadingLanguageName << std::endl;
		std::cout << std::endl;
		std::cout << "swap-chain:" << std::endl;
		std::cout << "  resolution:         " << swapChainRes.width << " x " << swapChainRes.height << std::endl;
		std::cout << "  samples:            " << mSwapChain->GetSamples() << std::endl;
		std::cout << "  colorFormat:        " << LLGL::ToString(mSwapChain->GetColorFormat()) << std::endl;
		std::cout << "  depthStencilFormat: " << LLGL::ToString(mSwapChain->GetDepthStencilFormat()) << std::endl;
		std::cout << std::endl;
		// ============================
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	// NOTE: Projection update must occur after debug shader is initialized
	UpdateProjection();
	_CreateWindow(title, inputHandler, transparent);
}

RenderContext::~RenderContext()
{
	mRenderSystem->Release(*mSwapChain);
	mSwapChain = nullptr;

	mRenderSystem->Release(*mCommands);
	mCommands = nullptr;
}

bool RenderContext::GetSurfaceNativeHandle(void* nativeHandle, std::size_t nativeHandleSize) const
{
	return mSwapChain->GetSurface().GetNativeHandle(nativeHandle, nativeHandleSize);
}

bool RenderContext::GetBackendNativeHandle(void* nativeHandle, std::size_t nativeHandleSize) const
{
	return mRenderSystem->GetNativeHandle(nativeHandle, nativeHandleSize);
}

bool RenderContext::GetCommandBufferNativeHandle(void* nativeHandle, std::size_t nativeHandleSize) const
{
	return mCommands->GetNativeHandle(nativeHandle, nativeHandleSize);
}

LLGL::Extent2D RenderContext::GetResolution() const
{
	return mSwapChain->GetResolution();
}

void RenderContext::SetBackgroundClearColor(const LLGL::ColorRGBAf color)
{
	mBackgroundColor = color;
}

void RenderContext::BeginFrame() const
{
	// Render Commands to Queue
	mCommands->Begin();

	mCommands->Clear(LLGL::ClearFlags::ColorDepth | LLGL::ClearFlags::Color, {
		                 mBackgroundColor.r, mBackgroundColor.g, mBackgroundColor.b, mBackgroundColor.a
	                 });
	// set viewport and scissor rectangle
	mCommands->SetViewport(mSwapChain->GetResolution());

	mCommands->BeginRenderPass(*mSwapChain);
}

void RenderContext::EndFrame() const
{
	mCommands->EndRenderPass();

	mCommands->End();
	mCommandQueue->Submit(*mCommands);

	// Present results on screen
	mSwapChain->Present();
}

bool RenderContext::ProcessEvents() const
{
	const LLGL::Window& window = LLGL::CastTo<LLGL::Window>(mSwapChain->GetSurface());
	return mSwapChain->GetSurface().ProcessEvents() && !window.HasQuit();
}

// Called on window resize
void RenderContext::UpdateProjection()
{
	const auto res = mSwapChain->GetResolution();
	const auto perspective = glm::perspective(glm::radians(45.0f),
	                                          static_cast<float>(res.width) / static_cast<float>(res.height),
	                                          0.1f, 100.0f);

	const auto ortho = glm::ortho(0.0f, static_cast<float>(res.width), static_cast<float>(res.height), 0.0f, 0.1f,
	                              100.0f);

	mProjection = mUsePerspective ? perspective : ortho;
}

glm::vec3 RenderContext::NormalizedDeviceCoords(glm::vec3 vec) const
{
	const auto res = mSwapChain->GetResolution();
	return {
		(2.0f * vec.x) / res.width - 1.0f,
		1.0f - (2.0f * vec.y) / res.height,
		vec.z
	};
}

glm::mat4 RenderContext::GetProjection() const
{
	return mProjection;
}

void RenderContext::ResizeBuffers(const LLGL::Extent2D& size) const
{
	mSwapChain->ResizeBuffers(size);
}

void RenderContext::_CreateWindow(const LLGL::UTF8String& title, const std::shared_ptr<InputHandler>& inputHandler,
                                  bool transparent)
{
	// get window from context surface
	auto& window = LLGL::CastTo<LLGL::Window>(mSwapChain->GetSurface());

	// Needs L outside of quotes?
	window.SetTitle(title);

	// Change window descriptor to allow resizing
	auto wndDesc = window.GetDesc();
	wndDesc.flags |= LLGL::WindowFlags::Visible | LLGL::WindowFlags::Resizable;
	if (transparent)
	{
		wndDesc.flags &= ~LLGL::WindowFlags::Borderless;
		wndDesc.flags &= ~LLGL::WindowFlags::Resizable;
	}
	window.SetDesc(wndDesc);

	// Add window resize listener
	window.AddEventListener(std::make_shared<ResizeEventHandler>(*this));
	window.AddEventListener(inputHandler);
	if (transparent)
	{
		LLGL::NativeHandle mainWindowHandle;
		GetSurfaceNativeHandle(&mainWindowHandle, sizeof(mainWindowHandle));

		SetWindowLong(mainWindowHandle.window, GWL_EXSTYLE,
		              GetWindowLong(mainWindowHandle.window, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetWindowPos(mainWindowHandle.window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

		SetLayeredWindowAttributes(mainWindowHandle.window, RGB(255, 0, 255), 0, LWA_COLORKEY);
	}

	window.Show();
}

LLGL::Extent2D RenderContext::_ScaleResolution(const LLGL::Extent2D& res, float scale)
{
	const float wScaled = static_cast<float>(res.width) * scale;
	const float hScaled = static_cast<float>(res.height) * scale;
	return LLGL::Extent2D
	{
		static_cast<std::uint32_t>(wScaled + 0.5f),
		static_cast<std::uint32_t>(hScaled + 0.5f)
	};
}
