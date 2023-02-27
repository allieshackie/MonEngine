#pragma once
#include <LLGL/LLGL.h>
#include <glm/mat4x4.hpp>

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

private:
	void _InitPipeline();
	void _UpdateUniforms(const TransformComponent& transform, const SpriteComponent& sprite) const;

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

	Renderer& mRenderer;
	ResourceManager& mResourceManager;
	EntityRegistry& mEntityRegistry;
};
