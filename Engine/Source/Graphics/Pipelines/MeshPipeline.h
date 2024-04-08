#pragma once
#include <glm/mat4x4.hpp>

#include "PipelineBase.h"
#include "Entity/Components/MeshComponent.h"

class EntityRegistry;
class Mesh;
class RenderObject;
class Shader;

struct TransformComponent;

struct MeshVBO
{
	LLGL::Buffer* mVertexBuffer = nullptr;
	LLGL::Buffer* mIndexBuffer = nullptr;
	int mNumIndices = 0;
};

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


private:
	int _SetMeshVBuffer(LLGL::CommandBuffer& commands, const MeshComponent& meshComponent);

	std::unordered_map<std::string, MeshVBO> mMeshVertexBuffers;

	MeshVBO mTestMesh;
};
