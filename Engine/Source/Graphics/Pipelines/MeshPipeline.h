#pragma once
#include <LLGL/LLGL.h>
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, perspective
#include <glm/mat4x4.hpp>

class RenderObject;
class Shader;

struct TriangleMesh
{
	std::uint32_t firstVertex = 0;
	std::uint32_t numVertices = 0;
	glm::mat4 transform;
	glm::vec4 color;
};

class MeshPipeline
{
public:
	void Init(LLGL::RenderSystemPtr& renderSystem, const std::string& shaderPath);
	void Render(LLGL::CommandBuffer& commands) const;

	void UpdateProjectionViewModelUniform(LLGL::CommandBuffer& commands, glm::mat4 model, glm::mat4 projection,
	                                      glm::mat4 view);

	void AddRenderObjectVBuffer(const LLGL::RenderSystemPtr& renderSystem, RenderObject& obj);

private:
	TriangleMesh _LoadObjModel(std::vector<TexturedVertex>& vertices, const std::string& filename) const;

	LLGL::PipelineState* mPipeline = nullptr;
	std::unique_ptr<Shader> mShader = nullptr;
	LLGL::Buffer* mConstantBuffer = nullptr;
	LLGL::Buffer* mVertexBuffer = nullptr;
	LLGL::ResourceHeap* mVolumeResourceHeap = nullptr;

	struct VolumeSettings
	{
		glm::mat4 pvmMat = glm::identity<glm::mat4>();
		LLGL::ColorRGBAf color = {1, 1, 1, 1};
	}
	volumeSettings = {};

	std::unique_ptr<Model> model = nullptr;
};
