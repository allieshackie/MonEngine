#include "LLGL/Misc/TypeNames.h"
#include <glm/ext/matrix_clip_space.hpp>

#include "ResourceManager.h"
#include "Debug/DebugDraw.h"

#include "Renderer.h"

static constexpr int SCREEN_WIDTH = 800;
static constexpr int SCREEN_HEIGHT = 600;

Renderer::Renderer(ResourceManager& resourceManager, EntityRegistry& entityRegistry)
	: mEntityRegistry(entityRegistry), mResourceManager(resourceManager)
{
	// Initialize default projection matrix
	_Init();
}

void Renderer::_Init()
{
	try
	{
		// Load render system module (hard code to OpenGL for now)
		mRenderer = LLGL::RenderSystem::Load("OpenGL");

		// Render context (window attributes)
		LLGL::SwapChainDescriptor swapChainDesc;
		{
			swapChainDesc.resolution = {SCREEN_WIDTH, SCREEN_HEIGHT};
#ifdef ENABLE_MULTISAMPLING
            contextDesc.samples = 8;
#endif
		}
		mSwapChain = mRenderer->CreateSwapChain(swapChainDesc);
		mSwapChain->SetVsyncInterval(1);

		LLGL::CommandBufferDescriptor cmdBufferDesc;
		{
			cmdBufferDesc.flags = LLGL::CommandBufferFlags::ImmediateSubmit;
		}
		// Create command buffer to submit graphics commands
		mCommands = mRenderer->CreateCommandBuffer(cmdBufferDesc);

		// Get command queue to record and submit command buffers
		mCommandQueue = mRenderer->GetCommandQueue();

		// ==== TODO: Debug info ====
		// Print renderer information
		const auto& info = mRenderer->GetRendererInfo();
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

	mResourceManager.LoadAllTexturesFromFolder(*mRenderer);
	mPipeline2D = std::make_unique<Pipeline2D>(*this, mResourceManager, mEntityRegistry);
	// TODO: Enable for 3D
	//mPipeline3D = std::make_unique<Pipeline3D>(*this, mResourceManager);
	mDebugPipeline = std::make_unique<DebugPipeline>(*this, mResourceManager);
	// NOTE: Projection update must occur after debug shader is initialized
	UpdateProjection();
}

void Renderer::OnDrawFrame(const std::function<void()>& drawCallback) const
{
	// Render Commands to Queue
	mCommands->Begin();
	{
		// clear color buffer
		mCommands->Clear(LLGL::ClearFlags::Color);
		// set viewport and scissor rectangle
		mCommands->SetViewport(mSwapChain->GetResolution());

		// set the render context as the initial render target
		mCommands->BeginRenderPass(*mSwapChain);
		{
			mPipeline2D->Tick();
			mDebugPipeline->Render(*mCommands);
			// TODO: Enable for 3D
			//mPipeline3D->Render(*mCommands);
			// gui draw calls, this can include images so we want it to
			// piggyback off the pipeline change in the sprite draw call
			drawCallback();
		}

		mCommands->EndRenderPass();
	}
	mCommands->End();
	mCommandQueue->Submit(*mCommands);

	// Present results on screen
	mSwapChain->Present();
}

// Called on window resize
void Renderer::UpdateProjection()
{
	const auto res = mSwapChain->GetResolution();
	mProjection = glm::perspective(glm::radians(45.0f),
	                               static_cast<float>(res.width) / static_cast<float>(res.height),
	                               0.1f, 100.0f);
}

void Renderer::UpdateView(glm::mat4 view)
{
	mView = view;
}

glm::mat4 Renderer::GetProjection() const
{
	return mProjection;
}

glm::mat4 Renderer::GetView() const
{
	return mView;
}

glm::vec3 Renderer::NormalizedDeviceCoords(glm::vec3 vec) const
{
	const auto res = mSwapChain->GetResolution();
	return {(2.0f * vec.x) / res.width - 1.0f, 1.0f - (2.0f * vec.y) / res.height, vec.z};
}
