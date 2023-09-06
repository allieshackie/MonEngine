#pragma once
#include <LLGL/LLGL.h>
#include <glm/mat4x4.hpp>

#include "Core/ResourceManager.h"
#include "Core/Vertex.h"
#include "Core/Shader.h"

struct SpriteComponent;
struct TransformComponent;

class EntityRegistry;

class SpritePipeline
{
public:
	SpritePipeline() = default;

	~SpritePipeline()
	{
		std::cout << "Delete SpritePipeline" << std::endl;
	}

	void Init(std::unique_ptr<LLGL::RenderSystem>& renderSystem, const std::string& shaderPath,
	          const TextureMap& textures);
	void Render(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat, EntityRegistry& entityRegistry) const;

private:
	void _CreateResourceHeap(const std::unique_ptr<LLGL::RenderSystem>& renderSystem, const TextureMap& textures);

	void _Render(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat, const TransformComponent& transform,
	             const SpriteComponent& sprite) const;
	void _UpdateUniforms(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat,
	                     const TransformComponent& transform) const;

	LLGL::PipelineLayout* mPipelineLayout = nullptr;
	LLGL::PipelineState* mPipeline = nullptr;
	LLGL::ResourceHeap* mResourceHeap = nullptr;

	std::unique_ptr<Shader> mShader = nullptr;

	LLGL::Buffer* mConstantBuffer = nullptr;
	LLGL::Buffer* mVertexBuffer = nullptr;

	struct SpriteSettings
	{
		glm::mat4 pvmMat;
		glm::mat4 textureTransform;
	}
	spriteSettings = {};

	std::vector<Vertex> mVertices = {
		{{-0.5, -0.5, 1}, {1, 1, 1}, {0, 1}}, // top left
		{{-0.5, 0.5, 1}, {1, 1, 1}, {0, 0}}, // bottom left
		{{0.5, -0.5, 1}, {1, 1, 1}, {1, 1}}, // top right
		{{0.5, 0.5, 1}, {1, 1, 1}, {1, 0}}, // bottom right
	};
};
