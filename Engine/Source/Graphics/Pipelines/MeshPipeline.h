#pragma once
#include <glm/mat4x4.hpp>

#include "PipelineBase.h"
#include "Graphics/Core/Model.h"
#include "Entity/Components/MeshComponent.h"

class EntityRegistry;
class RenderObject;
class Shader;

struct TransformComponent;

class MeshPipeline : public PipelineBase
{
public:
	MeshPipeline(LLGL::RenderSystemPtr& renderSystem);

	void Render(LLGL::CommandBuffer& commands, const glm::mat4 pvMat, EntityRegistry& entityRegistry);
	void RenderMap(LLGL::CommandBuffer& commands, const glm::mat4 pvMat, const MeshComponent& meshComponent,
	               const TransformComponent& transform);

	void SetPipeline(LLGL::CommandBuffer& commands) const;

	void UpdateProjectionViewModelUniform(LLGL::CommandBuffer& commands, glm::mat4 pvMat,
	                                      const TransformComponent& transform) const;

	void SetResourceHeapTexture(LLGL::CommandBuffer& commands, int textureId) const;

private:
	void _RenderModel(LLGL::CommandBuffer& commands, const MeshComponent& meshComponent, glm::mat4 pvMat,
	                  const TransformComponent& transform);

	std::unordered_map<std::string, Model> mModelVertexBuffers;
};
