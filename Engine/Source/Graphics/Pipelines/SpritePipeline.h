#pragma once
#include <LLGL/LLGL.h>
#include <glm/mat4x4.hpp>

#include "PipelineBase.h"
#include "Graphics/Core/Vertex.h"

struct SpriteComponent;
struct TransformComponent;

class EntityRegistry;

class SpritePipeline : public PipelineBase
{
public:
	SpritePipeline(const LLGL::RenderSystemPtr& renderSystem);

	void Render(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat, EntityRegistry& entityRegistry) const;

private:
	void _Render(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat, const TransformComponent& transform,
	             const SpriteComponent& sprite) const;
	void _UpdateUniforms(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat,
	                     const TransformComponent& transform) const;

	LLGL::Buffer* mVertexBuffer = nullptr;

	std::vector<Vertex> mVertices = {
		{{-0.5, -0.5, 1}, {1, 1, 1, 1}, {0, 1}}, // top left
		{{-0.5, 0.5, 1}, {1, 1, 1, 1}, {0, 0}}, // bottom left
		{{0.5, -0.5, 1}, {1, 1, 1, 1}, {1, 1}}, // top right
		{{0.5, 0.5, 1}, {1, 1, 1, 1}, {1, 0}}, // bottom right
	};
};
