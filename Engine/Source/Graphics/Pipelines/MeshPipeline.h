#pragma once
#include <LLGL/LLGL.h>
#include <glm/mat4x4.hpp>
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

class MeshPipeline
{
public:
	void Init(LLGL::RenderSystemPtr& renderSystem);

	void Render(LLGL::CommandBuffer& commands, const glm::mat4 pvMat, EntityRegistry& entityRegistry);
	void RenderMap(LLGL::CommandBuffer& commands, const glm::mat4 pvMat, const MeshComponent& meshComponent,
	               const TransformComponent& transform);

	void SetPipeline(LLGL::CommandBuffer& commands) const;

	void UpdateProjectionViewModelUniform(LLGL::CommandBuffer& commands, glm::mat4 pvMat,
	                                      const TransformComponent& transform) const;


private:
	int _SetMeshVBuffer(LLGL::CommandBuffer& commands, const MeshComponent& meshComponent);

	LLGL::PipelineState* mPipeline = nullptr;
	std::unique_ptr<Shader> mShader = nullptr;
	LLGL::ResourceHeap* mVolumeResourceHeap = nullptr;

	LLGL::Buffer* mConstantBuffer = nullptr;

	std::unordered_map<std::string, MeshVBO> mMeshVertexBuffers;

	MeshVBO mTestMesh;

	struct VolumeSettings
	{
		glm::mat4 pvmMat = glm::mat4();
		glm::mat4 mMat = glm::mat4();
	}
	mVolumeSettings = {};
};
