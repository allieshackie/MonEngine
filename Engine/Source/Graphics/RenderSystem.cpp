#include <LLGL/LLGL.h>
#include "Core/World.h"
#include "WindowContext.h"

#include "RenderSystem.h"

RenderSystem::RenderSystem()
{
	try
	{
		LLGL::Report report;
		// Load render system module (hard code to OpenGL for now)
		mSystem = LLGL::RenderSystem::Load("OpenGL", &report);
		// Set report callback to standard output
		LLGL::Log::RegisterCallbackStd();

		const auto display = LLGL::Display::GetPrimary();

		LLGL::CommandBufferDescriptor cmdBufferDesc;
		{
			cmdBufferDesc.flags = LLGL::CommandBufferFlags::ImmediateSubmit;
		}
		// Create command buffer to submit graphics commands
		mCommands = mSystem->CreateCommandBuffer(cmdBufferDesc);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

RenderSystem::~RenderSystem()
{
	mSystem->Release(*mCommands);
	mCommands = nullptr;
}

void RenderSystem::LoadFont(const char* fontFileName) const
{
	mTextPipeline->LoadFont(fontFileName);
}

void RenderSystem::Render(std::weak_ptr<World> world)
{
	auto worldPtr = world.lock();
	if (worldPtr == nullptr)
	{
		return;
	}
	const auto projectionViewMat = mPerspectiveProjection * worldPtr->GetCamera().GetView();

	mMeshPipeline->Render(*mCommands, mPerspectiveProjection, world);
	mTextPipeline->Render(*mCommands, projectionViewMat);
	mImmediatePipeline->Render(*mCommands, projectionViewMat);
	mOverlayPipeline->Render(*mCommands, mOrthoProjection);
}

void RenderSystem::ClearOverlay() const
{
	mOverlayPipeline->ClearOverlays();
}

int RenderSystem::AddOverlay(const std::vector<DebugVertex>& vertices, glm::mat4 transform) const
{
	return mOverlayPipeline->AddOverlay(vertices, transform);
}

void RenderSystem::UpdateOverlayTransform(int id, glm::mat4 transform) const
{
	return mOverlayPipeline->UpdateOverlayTransform(id, transform);
}

void RenderSystem::DrawPoint(glm::vec3 pos, glm::vec4 color, float size) const
{
	mImmediatePipeline->DrawPoint(pos, color, size);
}

void RenderSystem::DrawLine(glm::vec3 from, glm::vec3 to, glm::vec4 color) const
{
	mImmediatePipeline->DrawLine(from, to, color);
}

void RenderSystem::DrawBox(glm::vec3 pos, glm::vec3 size, glm::vec4 color, bool filled) const
{
	mImmediatePipeline->DrawBox(pos, size, color, filled);
}

void RenderSystem::DrawCircle(glm::vec3 position, float radius, glm::vec4 color) const
{
	mImmediatePipeline->DrawCircle(position, radius, color);
}

void RenderSystem::DrawGrid() const
{
	mImmediatePipeline->DrawGrid();
}

void RenderSystem::DrawOverlayLine(glm::vec3 from, glm::vec3 to, glm::vec4 color) const
{
	mOverlayPipeline->DrawLine(from, to, color);
}

uint32_t RenderSystem::AddText(const char* text, glm::vec2 position, glm::vec2 size, glm::vec4 color) const
{
	return mTextPipeline->AddText(text, position, size, color);
}

void RenderSystem::UpdateText(uint32_t index, const std::string& text)
{
	mTextPipeline->UpdateText(index, text);
}

void RenderSystem::SetVisible(uint32_t index, bool visible)
{
	mTextPipeline->SetVisible(index, visible);
}

void RenderSystem::SetPosition(uint32_t index, glm::vec2 pos)
{
	mTextPipeline->SetPosition(index, pos);
}

void RenderSystem::RemoveText(uint32_t index)
{
	mTextPipeline->RemoveText(index);
}

Material& RenderSystem::GetMaterial()
{
	return mMeshPipeline->GetMaterial();
}

void RenderSystem::UpdateLights()
{
	mMeshPipeline->UpdateLights();
}

void RenderSystem::UpdateProjections(const LLGL::Extent2D res)
{
	mPerspectiveProjection = glm::perspective(glm::radians(45.0f),
		static_cast<float>(res.width) / static_cast<float>(res.height),
		0.1f, 100.0f);

	mOrthoProjection = glm::ortho(0.0f, static_cast<float>(res.width), 0.0f, static_cast<float>(res.height), 0.1f,
		100.0f);
}

bool RenderSystem::GetBackendNativeHandle(void* nativeHandle, std::size_t nativeHandleSize) const
{
	return mSystem->GetNativeHandle(nativeHandle, nativeHandleSize);
}

bool RenderSystem::GetCommandBufferNativeHandle(void* nativeHandle, std::size_t nativeHandleSize) const
{
	return mCommands->GetNativeHandle(nativeHandle, nativeHandleSize);
}

void RenderSystem::OnWorldCreated(std::weak_ptr<World> world)
{
	mMeshPipeline->OnWorldCreated(world);
}

void RenderSystem::OnWindowCreated(ResourceManager& resourceManager)
{
	// Get command queue to record and submit command buffers
	mCommandQueue = mSystem->GetCommandQueue();

	mImmediatePipeline = std::make_unique<ImmediatePipeline>(mSystem);
	mTextPipeline = std::make_unique<TextPipeline>(mSystem);
	mMeshPipeline = std::make_unique<MeshPipeline>(mSystem, resourceManager);
	mOverlayPipeline = std::make_unique<OverlayPipeline>(mSystem);
}