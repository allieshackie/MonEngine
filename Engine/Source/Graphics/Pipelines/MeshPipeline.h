#pragma once
#include <glm/mat4x4.hpp>

#include "PipelineBase.h"
#include "Entity/EntityRegistry.h"
#include "Entity/Components/MeshComponent.h"
#include "Graphics/Core/Model.h"
#include "Graphics/Core/ResourceManager.h"

#define MAX_LIGHTS 4
#define MAX_BONES 40

class Animator;
class Camera;
class EntityRegistry;
class RenderObject;
class Shader;

struct TransformComponent;

class MeshPipeline : public PipelineBase
{
public:
	MeshPipeline(LLGL::RenderSystemPtr& renderSystem, EntityRegistry& entityRegistry,
	             const ResourceManager& resourceManager);

	void Render(LLGL::CommandBuffer& commands, const Camera& camera, const glm::mat4 projection,
	            EntityRegistry& entityRegistry, ResourceManager& resourceManager,
	            const LLGL::RenderSystemPtr& renderSystem);
	void RenderMap(LLGL::CommandBuffer& commands, const Camera& camera, const LLGL::RenderSystemPtr& renderSystem,
	               const ResourceManager& resourceManager, const glm::mat4 projection,
	               MeshComponent& meshComponent, const TransformComponent& transform);

	void SetPipeline(LLGL::CommandBuffer& commands) const;

	void UpdateProjectionViewModelUniform(LLGL::CommandBuffer& commands, const Camera& camera,
	                                      const LLGL::RenderSystemPtr& renderSystem, const glm::mat4 projection,
	                                      const TransformComponent& transform, MeshComponent& mesh);

	void SetResourceHeapTexture(LLGL::CommandBuffer& commands, LLGL::Texture& texture) const;

	void UpdateLightBuffer(const LLGL::RenderSystemPtr& renderSystem) const;
	void AddLight(EnTTRegistry& registry, EntityId entity);

private:
	void _RenderModel(LLGL::CommandBuffer& commands, MeshComponent& meshComponent, const Camera& camera,
	                  const LLGL::RenderSystemPtr& renderSystem, const ResourceManager& resourceManager,
	                  const glm::mat4 projection, const TransformComponent& transform);

	void _ProcessLights(EntityRegistry& entityRegistry);

	LLGL::Buffer* mLightBuffer = nullptr;
	LLGL::Buffer* mMaterialBuffer = nullptr;
	LLGL::Buffer* mBoneBuffer = nullptr;

	struct MeshSettings
	{
		// projection-view-model matrix
		glm::mat4 model = glm::mat4();
		glm::mat4 view = glm::mat4();
		glm::mat4 projection = glm::mat4();
		// texture clip to render part of texture
		glm::mat4 textureClip = glm::mat4();
		glm::vec4 hasBones = {0, 0, 0, 0};
	}
	meshSettings = {};

	struct LightSettings
	{
		glm::vec3 viewPos = {0, 0, 0};
		int numLights = 0;
	}
	lightSettings = {};

	LLGL::Buffer* mLightConstantBuffer = nullptr;

	struct Material // TODO: Might need to be fixed for 16 byte alignment (emission could be vec3)
	{
		glm::vec4 ambient = {0, 0, 0, 0};
		glm::vec4 diffuse = {0, 0, 0, 0};
		glm::vec4 specular = {0, 0, 0, 0};
		glm::vec4 emission = {0, 0, 0, 0};
		float shininess = 0.0f;
	};

	Material material = {
		{1.0, 1.0, 1.0, 1.0},
		{0.8, 0.8, 0.8, 1.0},
		{0.5, 0.5, 0.5, 1.0},
		{0.0, 0.0, 0.0, 1.0},
		359.9f
	};

	struct LightUniform
	{
		glm::vec3 position = {0, 0, 0};
		float intensity = 1.0f;
		glm::vec4 color = {0, 0, 0, 0};
	};

	std::vector<LightUniform> mLights;

	std::vector<EntityId> mQueuedLightEntities;
};
