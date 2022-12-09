#pragma once
#include <LLGL/LLGL.h>
#include <glm/mat4x4.hpp>
#include "Core/Vertex.h"

class RenderObject;
class Shader;
class Renderer;
class ResourceManager;

class Pipeline2D
{
public:
	Pipeline2D(Renderer& renderer, ResourceManager& resourceManager);
	~Pipeline2D() = default;

	void Render(LLGL::CommandBuffer& commands) const;

	void UpdateProjectionViewModelUniform(LLGL::CommandBuffer& commands, glm::mat4 model, glm::mat4 projection,
	                                      glm::mat4 view);
	void UpdateTextureClipUniform(LLGL::CommandBuffer& commands, glm::mat4 textureClip);

	void AddRenderObjectVBuffer(RenderObject& obj) const;

private:
	void _InitPipeline();

	LLGL::PipelineState* mPipeline = nullptr;
	Shader* mShader = nullptr;
	LLGL::Buffer* mConstantBuffer = nullptr;
	LLGL::Buffer* mVertexBuffer = nullptr;

	struct SpriteSettings
	{
		glm::mat4 pvmMat;
		glm::mat4 textureClip;
	}
	spriteSettings = {};

	// Use 0.5 because we want center to be 0,0 and local size to total to 1
	const std::vector<Vertex> mVertices = {
		{{-0.5, -0.5, 1}, {1, 1, 1}, {0, 0}}, // top left
		{{-0.5, 0.5, 1}, {1, 1, 1}, {0, 1}}, // bottom left
		{{0.5, -0.5, 1}, {1, 1, 1}, {1, 0}}, // top right
		{{0.5, 0.5, 1}, {1, 1, 1}, {1, 1}}, // bottom right
	};

	Renderer& mRenderer;
	ResourceManager& mResourceManager;
};
