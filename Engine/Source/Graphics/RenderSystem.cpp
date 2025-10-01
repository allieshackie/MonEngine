#include "Core/World.h"
#include "RenderContext.h"

#include "RenderSystem.h"

RenderSystem::RenderSystem(RenderContext& context, const ResourceManager& resourceManager) : mContext(context)
{
	mImmediatePipeline = std::make_unique<ImmediatePipeline>(mContext.GetRenderSystem());
	mTextPipeline = std::make_unique<TextPipeline>(mContext.GetRenderSystem());
	mMeshPipeline = std::make_unique<MeshPipeline>(mContext.GetRenderSystem(), resourceManager);
	mOverlayPipeline = std::make_unique<OverlayPipeline>(mContext.GetRenderSystem());
}

void RenderSystem::LoadFont(const char* fontFileName) const
{
	mTextPipeline->LoadFont(fontFileName);
}

void RenderSystem::Render(World* world) const
{
	const auto projectionViewMat = mContext.GetProjection() * world->GetCamera().GetView();

	mMeshPipeline->Render(mContext.GetCommands(), mContext.GetProjection(), world);
	mOverlayPipeline->Render(mContext.GetCommands());
	mImmediatePipeline->Render(mContext.GetCommands(), projectionViewMat);
	//mTextPipeline->Render(*mCommands, projectionViewMat);
	// TODO: debug shader is loading in for some meshes for some reason? Timing maybe?

	//mTextPipeline->Release(mRenderSystem);
}

void RenderSystem::ClearOverlay() const
{
	mOverlayPipeline->ClearOverlay();
}

void RenderSystem::SetSceneCallbacks(const World* world) const
{
	mMeshPipeline->SetSceneCallbacks(world);
}

void RenderSystem::DrawTextFont(const char* text, glm::vec2 position, glm::vec2 size, glm::vec4 color) const
{
	mTextPipeline->CreateTextMesh(text, position, size, color);
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

void RenderSystem::DrawOverlay(glm::vec2 pos, glm::vec4 color) const
{
	mOverlayPipeline->DrawOverlay(pos, color);
}

void RenderSystem::DrawOverlayLine(glm::vec2 from, glm::vec2 to, glm::vec4 color) const
{
	mOverlayPipeline->DrawLine(from, to, color);
}
