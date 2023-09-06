#include "LLGL/Misc/TypeNames.h"
#include <glm/ext/matrix_clip_space.hpp>

#include "ResourceManager.h"
#include "Core/Camera.h"
#include "Graphics/Debug/DebugDraw.h"
#include "Input/InputHandler.h"

#include "RenderContext.h"

static constexpr int SCREEN_WIDTH = 800;
static constexpr int SCREEN_HEIGHT = 600;

void RenderContext::Init(const std::string& shaderPath, ResourceManager& resourceManager,
                         std::shared_ptr<InputHandler>& inputHandler)
{
	try
	{
		// Load render system module (hard code to OpenGL for now)
		mRenderSystem = LLGL::RenderSystem::Load("OpenGL");

		// Render context (window attributes)
		LLGL::SwapChainDescriptor swapChainDesc;
		{
			swapChainDesc.resolution = {SCREEN_WIDTH, SCREEN_HEIGHT};
#ifdef ENABLE_MULTISAMPLING
            contextDesc.samples = 8;
#endif
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

	resourceManager.LoadAllTexturesFromFolder(*mRenderSystem);
	// NOTE: Projection update must occur after debug shader is initialized
	UpdateProjection();

	mSpritePipeline = std::make_unique<SpritePipeline>();
	mMapPipeline = std::make_unique<MapPipeline>();
	mDebugPipeline = std::make_unique<DebugPipeline>();
	mTextPipeline = std::make_unique<TextPipeline>();
	// TODO: Enable for 3D
	//mPipeline3D = std::make_unique<Pipeline3D>(*this, mResourceManager);

	mSpritePipeline->Init(mRenderSystem, shaderPath, resourceManager.getTextures());
	mMapPipeline->Init(mRenderSystem, shaderPath, resourceManager.getTextures());
	mDebugPipeline->Init(mRenderSystem, shaderPath);
	mTextPipeline->Init(mRenderSystem, shaderPath, resourceManager.getTextures());
	//mPipeline3D->Init(mRenderSystem, shaderPath, resourceManager.getTextures());

	_InitWindow(inputHandler);
}

void RenderContext::LoadFont(const std::string& fontPath, const char* fontFileName) const
{
	mTextPipeline->LoadFont(mRenderSystem, fontPath, fontFileName);
}

bool RenderContext::GetNativeHandle(void* nativeHandle, std::size_t nativeHandleSize) const
{
	return mSwapChain->GetSurface().GetNativeHandle(nativeHandle, nativeHandleSize);
}

void RenderContext::BeginFrame() const
{
	// Render Commands to Queue
	mCommands->Begin();

	mCommands->Clear(LLGL::ClearFlags::Color);
	// set viewport and scissor rectangle
	mCommands->SetViewport(mSwapChain->GetResolution());

	// TODO: Can we generate and save map textures?
	//mMapPipeline->WriteQueuedMapTextures();

	mCommands->BeginRenderPass(*mSwapChain);
}

void RenderContext::Render(const std::unique_ptr<Camera>& camera, EntityRegistry& entityRegistry) const
{
	const auto perspectiveViewMat = mPerspectiveProjection * camera->GetView();
	const auto orthoViewMat = mPerspectiveProjection * camera->GetView();

	// TODO: Pass EntityRegistry
	// TODO: Call GUI draw manually in EngineContext!

	//mMapPipeline->Render(*mCommands, perspectiveViewMat);
	mTextPipeline->Render(*mCommands, orthoViewMat);
	mSpritePipeline->Render(*mCommands, perspectiveViewMat, entityRegistry);
	mDebugPipeline->Render(*mCommands, perspectiveViewMat);
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

void RenderContext::_InitWindow(std::shared_ptr<InputHandler>& inputHandler) const
{
	// get window from context surface
	auto& window = LLGL::CastTo<LLGL::Window>(mSwapChain->GetSurface());

	// Needs L outside of quotes?
	window.SetTitle(L"Editor");

	// Change window descriptor to allow resizing
	auto wndDesc = window.GetDesc();
	wndDesc.resizable = true;
	window.SetDesc(wndDesc);

	// Change window behavior
	const LLGL::WindowBehavior newBehavior = {true, 1};
	window.SetBehavior(newBehavior);

	// Add window resize listener
	//window.AddEventListener(std::make_shared<ResizeEventHandler>(mRenderer, *this, &mRenderer.GetSwapChain()));

	window.Show();

	window.AddEventListener(inputHandler);
}
