#include <glm/ext/matrix_clip_space.hpp>
#include "ResourceManager.h"
#include "RenderObjects/DebugDraw.h"

#include "Renderer.h"

static constexpr int SCREEN_WIDTH = 800;
static constexpr int SCREEN_HEIGHT = 600;

Renderer::Renderer(ResourceManager& resourceManager): mResourceManager(resourceManager)
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
		LLGL::RenderContextDescriptor contextDesc;
		{
			contextDesc.videoMode.resolution = {SCREEN_WIDTH, SCREEN_HEIGHT};
			contextDesc.vsync.enabled = true;
#ifdef ENABLE_MULTISAMPLING
            contextDesc.samples = 8;
#endif
		}
		mContext = mRenderer->CreateRenderContext(contextDesc);

		// Get command queue to record and submit command buffers
		mCommandQueue = mRenderer->GetCommandQueue();

		// Create command buffer to submit graphics commands
		mCommands = mRenderer->CreateCommandBuffer();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	mResourceManager.LoadAllTexturesFromFolder(*mRenderer);
	mPipeline2D = std::make_unique<Pipeline2D>(*this, mResourceManager);
	// TODO: Enable for 3D
	//mPipeline3D = std::make_unique<Pipeline3D>(*mRenderer);
	mDebugPipeline = std::make_unique<DebugPipeline>(*this, mResourceManager);
	// NOTE: Projection update must occur after debug shader is initialized
	UpdateProjection();
}

void Renderer::OnDrawFrame(const std::function<void()>& drawCallback) const
{
	// Render Commands to Queue
	mCommands->Begin();
	{
		// set viewport and scissor rectangle
		mCommands->SetViewport(mContext->GetResolution());

		// set the render context as the initial render target
		mCommands->BeginRenderPass(*mContext);
		{
			// clear color buffer
			mCommands->Clear(LLGL::ClearFlags::Color);
			mPipeline2D->Render(*mCommands);
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
	mContext->Present();
}

void Renderer::SetTexture(int textureId) const
{
	mResourceManager.SetTexture(*mCommands, textureId);
}

// Called on window resize
void Renderer::UpdateProjection()
{
	const auto res = mContext->GetVideoMode().resolution;
	mProjection = glm::perspective(glm::radians(45.0f),
	                               static_cast<float>(res.width) / static_cast<float>(res.height),
	                               0.1f, 1000.0f);
}

void Renderer::UpdateView(glm::mat4 view)
{
	mView = view;
}

glm::mat4 Renderer::GetProjection() const
{
	return mProjection;
}

glm::vec3 Renderer::NormalizedDeviceCoords(glm::vec3 vec) const
{
	const auto res = mContext->GetVideoMode().resolution;
	return {vec.x / res.width - 1.0f, 1.0f - vec.y / res.height, vec.z};
}

void Renderer::Update2DProjectionViewModelUniform(glm::mat4 model) const
{
	mPipeline2D->UpdateProjectionViewModelUniform(*mCommands, model, mProjection, mView);
}

void Renderer::Update2DTextureClipUniform(glm::mat4 textureClip) const
{
	mPipeline2D->UpdateTextureClipUniform(*mCommands, textureClip);
}

void Renderer::Add2DRenderObject(RenderObject& obj) const
{
	mPipeline2D->AddRenderObjectVBuffer(obj);
}

void Renderer::Update3DProjectionViewModelUniform(glm::mat4 model) const
{
	mPipeline3D->UpdateProjectionViewModelUniform(*mCommands, model, mProjection, mView);
}

void Renderer::Add3DRenderObject(RenderObject& obj) const
{
	mPipeline3D->AddRenderObjectVBuffer(obj);
}

void Renderer::UpdateDebugProjectionViewModelUniform(glm::mat4 model) const
{
	mDebugPipeline->UpdateProjectionViewModelUniform(*mCommands, model, mProjection, mView);
}

void Renderer::ClearDebugDraw() const
{
	mDebugPipeline->ClearDebugDraw();
}

void Renderer::AddDebugRenderObject(RenderObject& obj) const
{
	mDebugPipeline->AddRenderObjectVBuffer(obj);
}
