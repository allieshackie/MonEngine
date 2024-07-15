#pragma once
#include <glm/mat4x4.hpp>

#include "PipelineBase.h"
#include "Entity/EntityRegistry.h"
#include "Entity/Components/MeshComponent.h"
#include "Graphics/Core/Model.h"
#include "Graphics/Core/ResourceManager.h"

class Camera;
class EntityRegistry;
class RenderObject;
class Shader;

struct TransformComponent;

class MeshPipeline : public PipelineBase
{
public:
	MeshPipeline(LLGL::RenderSystemPtr& renderSystem, EntityRegistry& entityRegistry);

	void Render(LLGL::CommandBuffer& commands, const Camera& camera, const glm::mat4 projection,
	            EntityRegistry& entityRegistry, const LLGL::RenderSystemPtr& renderSystem);
	void RenderMap(LLGL::CommandBuffer& commands, const Camera& camera, const LLGL::RenderSystemPtr& renderSystem,
	               const glm::mat4 projection, const MeshComponent& meshComponent, const TransformComponent& transform);

	void SetPipeline(LLGL::CommandBuffer& commands) const;

	void UpdateProjectionViewModelUniform(LLGL::CommandBuffer& commands, const Camera& camera,
	                                      const LLGL::RenderSystemPtr& renderSystem, const glm::mat4 projection,
	                                      const TransformComponent& transform);

	void SetResourceHeapTexture(LLGL::CommandBuffer& commands, int textureId) const;

	void UpdateLightBuffer(const LLGL::RenderSystemPtr& renderSystem) const;
	void AddLight(EnTTRegistry& registry, EntityId entity);
	void RemoveLight(EnTTRegistry& registry, EntityId entity);

private:
	void _RenderModel(LLGL::CommandBuffer& commands, const MeshComponent& meshComponent, const Camera& camera,
	                  const LLGL::RenderSystemPtr& renderSystem,
	                  const glm::mat4 projection, const TransformComponent& transform);

	void _ProcessLights(EntityRegistry& entityRegistry);

	std::unordered_map<std::string, Model> mModelVertexBuffers;
	LLGL::Buffer* mLightBuffer = nullptr;
	LLGL::Buffer* mMaterialBuffer = nullptr;

	int mNumLights = 0;

	struct Settings
	{
		// projection-view-model matrix
		glm::mat4 model = glm::mat4();
		glm::mat4 view = glm::mat4();
		glm::mat4 projection = glm::mat4();
		// texture clip to render part of texture
		glm::mat4 textureClip = glm::mat4();
	}
	settings = {};

	struct Material
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
		glm::vec4 position = {0, 0, 0, 0};
		glm::vec4 color = {0, 0, 0, 0};
		float intensity = 1.0f;

		std::uint32_t entityId;
	};

	std::vector<LightUniform> mLights;
	std::vector<std::unique_ptr<Texture>> mTextures;

	std::vector<EntityId> mQueuedLightEntities;
};
