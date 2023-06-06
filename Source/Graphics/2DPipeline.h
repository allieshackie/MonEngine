#pragma once
#include <LLGL/LLGL.h>
#include <glm/mat4x4.hpp>
#include "Core/Vertex.h"
#include "Core/Shader.h"

struct SpriteComponent;
struct TransformComponent;

class EntityRegistry;
class LevelManager;
class Map;
class MapSystem;
class Renderer;
class ResourceManager;
class RenderObject;

class Pipeline2D
{
public:
	Pipeline2D(EntityRegistry& entityRegistry, LevelManager& levelManager, MapSystem& mapSystem, Renderer& renderer,
	           ResourceManager& resourceManager);

	~Pipeline2D()
	{
		std::cout << "Delete Pipeline2D" << std::endl;
	}

	void Tick() const;
	void Render(const TransformComponent& transform, const SpriteComponent& sprite) const;
	void RenderMap(const std::shared_ptr<Map>& map) const;
	void RenderMapTexture(const std::shared_ptr<Map>& map) const;

	void QueueWriteMapTexture(const std::shared_ptr<Map>& map);
	void WriteQueuedMapTextures();

private:
	void _InitPipeline();
	void _CreateResourceHeap();

	void _UpdateUniforms(const TransformComponent& transform) const;
	void _UpdateUniforms(glm::vec3 pos, glm::vec3 size, glm::vec3 rot) const;
	void _UpdateUniformsModel(glm::vec3 pos, glm::vec3 size, glm::vec3 rot, glm::vec4 texClip) const;

	void _InitMapTexturePipeline(std::shared_ptr<Map>& map);
	void _WriteMapTexture(std::shared_ptr<Map>& map) const;

	LLGL::PipelineLayout* mPipelineLayout = nullptr;
	LLGL::PipelineState* mPipeline = nullptr;
	LLGL::ResourceHeap* mResourceHeap = nullptr;

	std::unique_ptr<Shader> mShader;

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
	MapSystem& mMapSystem;
	Renderer& mRenderer;
	ResourceManager& mResourceManager;

	std::vector<std::shared_ptr<Map>> mQueuedMaps;
};
