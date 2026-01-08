#pragma once
#include <glm/mat4x4.hpp>

#include "PipelineBase.h"
#include "Entity/Components/LightComponent.h"
#include "Graphics/Core/ResourceManager.h"

#define MAX_LIGHTS 4
#define MAX_BONES 100

class Entity;
class World;

struct ModelComponent;
struct TransformComponent;

struct Material
{
	glm::vec4 emission = { 0, 0, 0, 0 };
	glm::vec4 params1 = { 0, 0, 0, 0 }; // x = shade_wrap, y = ambient_floor, z = spec_size, w = spec_strength
	glm::vec4 params2 = { 0, 0, 0, 0 }; // x = shininess, y = toon_steps, z = toon_smoothness
};

class MeshPipeline : public PipelineBase
{
public:
	MeshPipeline(const LLGL::RenderSystemPtr& renderSystem, const ResourceManager& resourceManager,
	             std::weak_ptr<World> world);

	void Render(LLGL::CommandBuffer& commands, const glm::mat4 projection, std::weak_ptr<World> world);

	void SetPipeline(LLGL::CommandBuffer& commands) const;
	void SetResourceHeapTexture(LLGL::CommandBuffer& commands, LLGL::Texture& texture) const;

	void UpdateLightBuffer() const;
	void AddLight(Entity* entity);

	Material& GetMaterial();
	void UpdateMaterialBuffer() const;

	void UpdateLights() { mUpdateLights = true; }

private:
	void _RenderNode(LLGL::CommandBuffer& commands, const Model& model, int nodeIndex,
	                 const TransformComponent& transform, const ModelComponent& modelComponent,
	                 std::shared_ptr<World> world);
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
		glm::vec4 params = { 0, 0, -1, 0 }; // x = hasTexture, y = hasBones, z = gTargetBone 
	} meshSettings = {};

	struct LightSettings
	{
		glm::vec3 viewPos = {0, 0, 0};
		int numLights = 0;
	} lightSettings = {};

	LLGL::Buffer* mLightConstantBuffer = nullptr;

	Material startingMaterial = {
		{0.0, 0.0, 0.0, 0.0},
		{0.35, 0.4, 11.0, 0.19},
		{56.0,2,0.07,0}
	};

	Material mCurrentMaterial = startingMaterial;

	struct LightUniform
	{
		glm::vec4 color = { 1,1,1,1 };
		glm::vec4 position = { 0,0,0,0 };
		glm::vec4 params = { 1.0f,0,0,0 }; // x = intensity, y = lightType
	};

	std::vector<LightUniform> mLights;
	bool mUpdateLights = false;

	std::vector<Entity*> mQueuedLightEntities;

	const LLGL::RenderSystemPtr& mRenderSystem;
	const ResourceManager& mResourceManager;
};
