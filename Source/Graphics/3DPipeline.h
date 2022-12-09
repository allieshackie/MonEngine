#pragma once
#include <LLGL/LLGL.h>
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, perspective
#include <glm/mat4x4.hpp>
#include "Core/Vertex.h"

class RenderObject;
class Shader;
class Renderer;
class ResourceManager;

class Pipeline3D
{
public:
	Pipeline3D(Renderer& renderer, ResourceManager& resourceManager);
	~Pipeline3D() = default;

	void Render(LLGL::CommandBuffer& commands) const;

	void UpdateProjectionViewModelUniform(LLGL::CommandBuffer& commands, glm::mat4 model, glm::mat4 projection,
	                                      glm::mat4 view);

	void AddRenderObjectVBuffer(RenderObject& obj);

private:
	void _InitPipeline();

	LLGL::PipelineState* mPipeline = nullptr;
	Shader* mShader = nullptr;
	LLGL::Buffer* mConstantBuffer = nullptr;
	LLGL::Buffer* mVertexBuffer = nullptr;
	LLGL::ResourceHeap* mVolumeResourceHeap = nullptr;

	struct VolumeSettings
	{
		glm::mat4 pvmMat = glm::identity<glm::mat4>();
		LLGL::ColorRGBAf color = {1, 1, 1, 1};
	}
	volumeSettings = {};

	Model* model = nullptr;

	Renderer& mRenderer;
	ResourceManager& mResourceManager;
};
