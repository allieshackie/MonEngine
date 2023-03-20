#pragma once
#include <LLGL/LLGL.h>
#include <glm/mat4x4.hpp>

#include "Core/Vertex.h"

struct MapComponent;
struct SpriteComponent;
struct TransformComponent;

class EntityRegistry;
class Renderer;
class ResourceManager;
class RenderObject;
class Shader;

class Pipeline2D
{
public:
	Pipeline2D(Renderer& renderer, ResourceManager& resourceManager, EntityRegistry& entityRegistry);
	~Pipeline2D();

	void Tick() const;
	void Render(const TransformComponent& transform, const SpriteComponent& sprite) const;
	void RenderMap(const TransformComponent& transform, const SpriteComponent& sprite, const MapComponent& map) const;

private:
	void _InitPipeline();
	void _UpdateUniforms(const TransformComponent& transform) const;
	void _UpdateUniforms(glm::vec3 pos, glm::vec3 size, float rot, glm::vec4 texClip) const;

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
		{{-0.5, -0.5, 1}, {1, 1, 1}, {0, 0}}, // top left
		{{-0.5, 0.5, 1}, {1, 1, 1}, {0, 1}}, // bottom left
		{{0.5, -0.5, 1}, {1, 1, 1}, {1, 0}}, // top right
		{{0.5, 0.5, 1}, {1, 1, 1}, {1, 1}}, // bottom right
	};

	Renderer& mRenderer;
	ResourceManager& mResourceManager;
	EntityRegistry& mEntityRegistry;
};
