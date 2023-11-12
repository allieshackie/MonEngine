#include "LLGL/Utils/TypeNames.h"
#include <glm/ext/matrix_clip_space.hpp>

#include "Core/Camera.h"
#include "Input/InputHandler.h"
#include "Map/MapRegistry.h"

#include "RenderContext.h"

RenderContext::RenderContext(const LLGL::UTF8String& title, const LLGL::Extent2D screenSize,
                             const LLGL::ColorRGBAf backgroundColor,
                             const std::shared_ptr<InputHandler>& inputHandler, bool usePerspective)
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

	_CreatePipelines();
	_CreateWindow(title, inputHandler);
}

void RenderContext::LoadFont(const char* fontFileName) const
{
	mTextPipeline->LoadFont(mRenderSystem, fontFileName);
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

void RenderContext::SetBackgroundClearColor(const LLGL::ColorRGBAf color)
{
	mBackgroundColor = color;
}

void RenderContext::BeginFrame() const
{
	// Render Commands to Queue
	mCommands->Begin();

	mCommands->Clear(LLGL::ClearFlags::Color, {
		                 mBackgroundColor.r, mBackgroundColor.g, mBackgroundColor.b, mBackgroundColor.a
	                 });
	// set viewport and scissor rectangle
	mCommands->SetViewport(mSwapChain->GetResolution());

	// TODO: Can we generate and save map textures?
	mMapPipeline->WriteQueuedMapTextures(mRenderSystem, *mCommands);

	mCommands->BeginRenderPass(*mSwapChain);
}

void RenderContext::Render(const std::unique_ptr<Camera>& camera, EntityRegistry& entityRegistry,
                           MapRegistry& mapRegistry) const
{
	const auto projectionViewMat = mProjection * camera->GetView();

	for (const auto& map : mapRegistry.GetAllMaps())
	{
		mMapPipeline->Render(*mCommands, projectionViewMat, map);
	}
	mTextPipeline->Render(*mCommands, projectionViewMat);
	mSpritePipeline->Render(*mCommands, projectionViewMat, entityRegistry);
	mImmediatePipeline->Render(*mCommands, projectionViewMat);
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

void RenderContext::DrawText(const char* text, glm::vec2 position, glm::vec2 size, glm::vec4 color)
{
	mTextPipeline->CreateTextMesh(mRenderSystem, text, position, size, color);
}

void RenderContext::DrawPoint(glm::vec3 pos, glm::vec3 color, float size) const
{
	mImmediatePipeline->DrawPoint(pos, color, size);
}

void RenderContext::DrawLine(glm::vec3 from, glm::vec3 to, glm::vec3 color) const
{
	mImmediatePipeline->DrawLine(from, to, color);
}

void RenderContext::DrawBox(glm::vec3 pos, glm::vec3 size, glm::vec3 color) const
{
	mImmediatePipeline->DrawBox(pos, size, color);
}

void RenderContext::DrawCircle(glm::vec3 position, float radius, glm::vec3 color) const
{
	mImmediatePipeline->DrawCircle(position, radius, color);
}

void RenderContext::DrawGrid(glm::vec3 pos, glm::vec3 size, glm::vec3 color, int rows, int columns) const
{
	mImmediatePipeline->DrawGrid(pos, size, color, rows, columns);
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
		(vec.x / (static_cast<float>(res.width) / 2.0f) - 1.0f),
		-1 * (vec.y / (static_cast<float>(res.height) / 2.0f) - 1.0f), vec.z
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

void RenderContext::_CreateWindow(const LLGL::UTF8String& title, const std::shared_ptr<InputHandler>& inputHandler)
{
	// get window from context surface
	auto& window = LLGL::CastTo<LLGL::Window>(mSwapChain->GetSurface());

	// Needs L outside of quotes?
	window.SetTitle(title);

	// Change window descriptor to allow resizing
	auto wndDesc = window.GetDesc();
	wndDesc.flags |= LLGL::WindowFlags::Resizable | LLGL::WindowFlags::DisableClearOnResize;
	window.SetDesc(wndDesc);

	// Add window resize listener
	window.AddEventListener(std::make_shared<ResizeEventHandler>(*this));
	window.AddEventListener(inputHandler);

	window.Show();
}

void RenderContext::_CreatePipelines()
{
	mSpritePipeline = std::make_unique<SpritePipeline>();
	mMapPipeline = std::make_unique<MapPipeline>();
	mImmediatePipeline = std::make_unique<ImmediatePipeline>();
	mTextPipeline = std::make_unique<TextPipeline>();
	// TODO: Enable for 3D
	//mPipeline3D = std::make_unique<Pipeline3D>(*this, mResourceManager);

	mSpritePipeline->Init(mRenderSystem);
	mMapPipeline->Init(mRenderSystem);
	mImmediatePipeline->Init(mRenderSystem);
	mTextPipeline->Init(mRenderSystem);
	//mPipeline3D->Init(mRenderSystem, shaderPath);
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
