#include "LLGL/Utils/TypeNames.h"
#include <glm/ext/matrix_clip_space.hpp>

#include "Core/Camera.h"
#include "Input/InputHandler.h"
#include "Map/MapRegistry.h"

#include "RenderContext.h"

static constexpr int SCREEN_WIDTH = 800;
static constexpr int SCREEN_HEIGHT = 600;

void RenderContext::Init(const std::shared_ptr<InputHandler>& inputHandler)
{
	try
	{
		LLGL::Report report;
		// Load render system module (hard code to OpenGL for now)
		mRenderSystem = LLGL::RenderSystem::Load("OpenGL", &report);

		if (!mRenderSystem)
		{
			LLGL::Log::Errorf("%s", report.GetText());
			return;
		}

		// Create swap-chain
		LLGL::SwapChainDescriptor swapChainDesc;
		{
			swapChainDesc.resolution = {SCREEN_WIDTH, SCREEN_HEIGHT};
			swapChainDesc.depthBits = 0; // We don't need a depth buffer for this example
			swapChainDesc.stencilBits = 0; // We don't need a stencil buffer for this example
		}
		mSwapChain = mRenderSystem->CreateSwapChain(swapChainDesc);
		mSwapChain->SetVsyncInterval(1);

		LLGL::CommandBufferDescriptor cmdBufferDesc;
		{
			cmdBufferDesc.flags = LLGL::CommandBufferFlags::ImmediateSubmit;
		}
		// Create command buffer to submit graphics commands
		mCommands = mRenderSystem->CreateCommandBuffer(cmdBufferDesc);

		// Get command queue to record and submit command buffers
		mCommandQueue = mRenderSystem->GetCommandQueue();

		// ==== TODO: Debug info ====
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

	_InitWindow(inputHandler);
}

void RenderContext::LoadFont(const char* fontFileName) const
{
	mTextPipeline->LoadFont(mRenderSystem, fontFileName);
}

bool RenderContext::GetNativeHandle(void* nativeHandle, std::size_t nativeHandleSize) const
{
	return mSwapChain->GetSurface().GetNativeHandle(nativeHandle, nativeHandleSize);
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
	const auto perspectiveViewMat = mPerspectiveProjection * camera->GetView();
	const auto orthoViewMat = mOrthoProjection * camera->GetView();

	for (const auto& map : mapRegistry.GetAllMaps())
	{
		mMapPipeline->Render(*mCommands, perspectiveViewMat, map);
	}
	mTextPipeline->Render(*mCommands, orthoViewMat);
	mSpritePipeline->Render(*mCommands, perspectiveViewMat, entityRegistry);
	mImmediatePipeline->Render(*mCommands, perspectiveViewMat);
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
	return mSwapChain->GetSurface().ProcessEvents();
}

void RenderContext::DrawText(const char* text, glm::vec2 position, glm::vec2 size)
{
	mTextPipeline->CreateTextMesh(mRenderSystem, text, position, size);
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
	mPerspectiveProjection = glm::perspective(glm::radians(45.0f),
	                                          static_cast<float>(res.width) / static_cast<float>(res.height),
	                                          0.1f, 100.0f);

	mOrthoProjection = glm::ortho(0.0f, static_cast<float>(res.width), static_cast<float>(res.height), 0.0f, 0.1f,
	                              100.0f);
}

glm::vec3 RenderContext::NormalizedDeviceCoords(glm::vec3 vec) const
{
	const auto res = mSwapChain->GetResolution();
	return {
		(vec.x / (static_cast<float>(res.width) / 2.0f) - 1.0f),
		-1 * (vec.y / (static_cast<float>(res.height) / 2.0f) - 1.0f), vec.z
	};
}

glm::mat4 RenderContext::GetPerspectiveProjection() const
{
	return mPerspectiveProjection;
}

void RenderContext::ResizeBuffers(const LLGL::Extent2D& size) const
{
	mSwapChain->ResizeBuffers(size);
}

void RenderContext::_InitWindow(const std::shared_ptr<InputHandler>& inputHandler)
{
	// get window from context surface
	auto& window = LLGL::CastTo<LLGL::Window>(mSwapChain->GetSurface());

	// Needs L outside of quotes?
	window.SetTitle(L"Editor");

	// Change window descriptor to allow resizing
	auto wndDesc = window.GetDesc();
	wndDesc.flags |= LLGL::WindowFlags::Resizable | LLGL::WindowFlags::DisableClearOnResize;
	window.SetDesc(wndDesc);

	// Add window resize listener
	window.AddEventListener(std::make_shared<ResizeEventHandler>(*this));
	window.AddEventListener(inputHandler);

	window.Show();
}
