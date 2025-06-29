#pragma once
#include <glm/mat4x4.hpp>

#include "PipelineBase.h"
#include "Entity/Components/LightComponent.h"
#include "Entity/Components/ModelComponent.h"
#include "Graphics/Core/Model.h"
#include "Graphics/Core/ResourceManager.h"

#define MAX_LIGHTS 4
#define MAX_BONES 100

class Animator;
class Camera;
class Entity;
class MonScene;
class SceneManager;
class RenderObject;
class Shader;

struct TransformComponent;

class MeshPipeline : public PipelineBase
{
public:
	MeshPipeline(const LLGL::RenderSystemPtr& renderSystem, const ResourceManager& resourceManager);

	void Render(LLGL::CommandBuffer& commands, const Camera& camera, const glm::mat4 projection,
	            MonScene* scene, ResourceManager& resourceManager,
	            const LLGL::RenderSystemPtr& renderSystem);

	void SetPipeline(LLGL::CommandBuffer& commands) const;

	void UpdateProjectionViewModelUniform(LLGL::CommandBuffer& commands, const Camera& camera,
	                                      const LLGL::RenderSystemPtr& renderSystem,
	                                      const TransformComponent& transform, ModelComponent& mesh,
	                                      const Model& meshModel);

	void SetResourceHeapTexture(LLGL::CommandBuffer& commands, LLGL::Texture& texture) const;

	void UpdateLightBuffer(const LLGL::RenderSystemPtr& renderSystem) const;
	void AddLight(Entity* entity);

	void SetSceneCallbacks(const SceneManager& sceneManager);

private:
	void _ProcessLights();

	LLGL::Buffer* mLightBuffer = nullptr;
	LLGL::Buffer* mMaterialBuffer = nullptr;
	LLGL::Buffer* mBoneBuffer = nullptr;

	LLGL::Buffer* mFrameBuffer = nullptr;

	struct FrameSettings
	{
		glm::mat4 projection = glm::mat4();
		glm::mat4 view = glm::mat4();
	} frameSettings = {};

	struct MeshSettings
	{
		glm::mat4 model = glm::mat4();
		float hasTexture = 0;
		float hasBones = 0;
		float gTargetBone = -1;
		float _padding = 0;
		glm::vec4 solidColor = {1, 1, 1, 1};
	} meshSettings = {};

	struct LightSettings
	{
		glm::vec3 viewPos = {0, 0, 0};
		int numLights = 0;
	} lightSettings = {};

	LLGL::Buffer* mLightConstantBuffer = nullptr;

	struct Material // TODO: Might need to be fixed for 16 byte alignment (emission could be vec3)
	{
		glm::vec4 ambient = {0, 0, 0, 0};
		glm::vec4 diffuse = {0, 0, 0, 0};
		glm::vec4 specular = {0, 0, 0, 0};
		glm::vec4 emission = {0, 0, 0, 0};
		float shininess = 0.0f;
		glm::vec3 _padding = {0, 0, 0};
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
		glm::vec4 ambient = {0, 0, 0, 0};
		glm::vec4 diffuse = {0, 0, 0, 0};
		glm::vec4 specular = {0, 0, 0, 0};
		glm::vec3 position = {0, 0, 0};
		float intensity = 1.0f;
		glm::vec4 lightType = {0, 0, 0, 0};
	};

	std::vector<LightUniform> mLights;

	std::vector<Entity*> mQueuedLightEntities;
};
