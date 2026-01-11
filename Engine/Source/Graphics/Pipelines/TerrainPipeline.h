#pragma once

#include "PipelineBase.h"
#include "Graphics/Core/ResourceManager.h"

class World;

class TerrainPipeline : public PipelineBase
{
public:
	TerrainPipeline(const LLGL::RenderSystemPtr& renderSystem, const ResourceManager& resourceManager,
		std::weak_ptr<World> world);

	void Render(LLGL::CommandBuffer& commands, const glm::mat4 projection, std::weak_ptr<World> world);

private:
	struct FrameSettings
	{
		glm::mat4 projection = glm::mat4();
		glm::mat4 view = glm::mat4();
	} frameSettings = {};

	const LLGL::RenderSystemPtr& mRenderSystem;
	const ResourceManager& mResourceManager;
};