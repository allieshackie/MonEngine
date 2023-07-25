#pragma once
#include <LLGL/LLGL.h>
#include <glm/mat4x4.hpp>
#include "Core/Vertex.h"
#include "Core/Shader.h"

struct SpriteComponent;
struct TransformComponent;

class EntityRegistry;
class LevelManager;
class Renderer;
class ResourceManager;
class RenderObject;

class SpritePipeline
{
public:
	SpritePipeline(EntityRegistry& entityRegistry, LevelManager& levelManager, Renderer& renderer,
	               ResourceManager& resourceManager, std::string shadersFolderPath);

	~SpritePipeline()
	{
		std::cout << "Delete SpritePipeline" << std::endl;
	}

	void Tick() const;
	void Render(const TransformComponent& transform, const SpriteComponent& sprite) const;

private:
	void _InitPipeline();
	void _CreateResourceHeap();

	void _UpdateUniforms(const TransformComponent& transform) const;

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

	EntityRegistry& mEntityRegistry;
	LevelManager& mLevelManager;
	Renderer& mRenderer;
	ResourceManager& mResourceManager;

	std::string mShadersFolderPath;
};
