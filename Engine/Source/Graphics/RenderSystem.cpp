#include "Core/World.h"
#include "RenderContext.h"

#include "RenderSystem.h"

RenderSystem::RenderSystem(RenderContext& context, const ResourceManager& resourceManager, std::weak_ptr<World> world) :
	mContext(context)
{
	mImmediatePipeline = std::make_unique<ImmediatePipeline>(mContext.GetRenderSystem());
	mTextPipeline = std::make_unique<TextPipeline>(mContext.GetRenderSystem());
	mMeshPipeline = std::make_unique<MeshPipeline>(mContext.GetRenderSystem(), resourceManager, world);
	mOverlayPipeline = std::make_unique<OverlayPipeline>(mContext.GetRenderSystem());
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
	const auto projectionViewMat = mContext.GetPerspectiveProjection() * worldPtr->GetCamera().GetView();

	mMeshPipeline->Render(mContext.GetCommands(), mContext.GetPerspectiveProjection(), world);
	//mTextPipeline->Render(mContext.GetCommands(), projectionViewMat);
	//mImmediatePipeline->Render(mContext.GetCommands(), projectionViewMat);
	//mOverlayPipeline->Render(mContext.GetCommands(), mContext.GetOrthoProjection());
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

std::shared_ptr<TextMesh> RenderSystem::DrawTextFont(const char* text, glm::vec2 position, glm::vec2 size, glm::vec4 color) const
{
	return mTextPipeline->CreateTextMesh(text, position, size, color);
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

Material& RenderSystem::GetMaterial()
{
	return mMeshPipeline->GetMaterial();
}

void RenderSystem::UpdateLights()
{
	mMeshPipeline->UpdateLights();
}
