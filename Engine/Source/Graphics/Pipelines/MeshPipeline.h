#pragma once
#include <glm/mat4x4.hpp>

class Mesh;
class RenderObject;
class Shader;

class MeshPipeline
{
public:
	void Init(LLGL::RenderSystemPtr& renderSystem);
	void Render(LLGL::CommandBuffer& commands, const glm::mat4 pvMat);

	void UpdateProjectionViewModelUniform(LLGL::CommandBuffer& commands, glm::mat4 model, glm::mat4 pvMat);

	void AddRenderObjectVBuffer(const LLGL::RenderSystemPtr& renderSystem, const Mesh& mesh);

private:
	LLGL::PipelineState* mPipeline = nullptr;
	std::unique_ptr<Shader> mShader = nullptr;
	LLGL::ResourceHeap* mVolumeResourceHeap = nullptr;

	LLGL::Buffer* mConstantBuffer = nullptr;
	LLGL::Buffer* mVertexBuffer = nullptr;
	LLGL::Buffer* mIndexBuffer = nullptr;

	struct VolumeSettings
	{
		glm::mat4 pvmMat = glm::mat4();
		glm::mat4 mMat = glm::mat4();
	}
	mVolumeSettings = {};
};
