#pragma once
#include <glm/mat4x4.hpp>

#include "PipelineBase.h"
#include "Graphics/Core/Model.h"
#include "Entity/Components/MeshComponent.h"

class Camera;
class EntityRegistry;
class RenderObject;
class Shader;

struct TransformComponent;

class MeshPipeline : public PipelineBase
{
public:
	MeshPipeline(LLGL::RenderSystemPtr& renderSystem);

	void Render(LLGL::CommandBuffer& commands, const Camera& camera, const glm::mat4 projection,
	            EntityRegistry& entityRegistry);
	void RenderMap(LLGL::CommandBuffer& commands, const Camera& camera, const glm::mat4 projection,
	               const MeshComponent& meshComponent, const TransformComponent& transform);

	void SetPipeline(LLGL::CommandBuffer& commands) const;

	void UpdateProjectionViewModelUniform(LLGL::CommandBuffer& commands, const Camera& camera,
	                                      const glm::mat4 projection, const TransformComponent& transform);

	void SetResourceHeapTexture(LLGL::CommandBuffer& commands, int textureId) const;

private:
	void _RenderModel(LLGL::CommandBuffer& commands, const MeshComponent& meshComponent, const Camera& camera,
	                  const glm::mat4 projection, const TransformComponent& transform);

	std::unordered_map<std::string, Model> mModelVertexBuffers;
};
