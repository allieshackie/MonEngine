#pragma once

class RenderSystem;
class ResourceManager;
class World;

struct TransformComponent;

class ObjectGUI
{
public:
	void RenderBonesGUI(std::weak_ptr<World> weakWorld, ResourceManager& resourceManager, entt::entity selectedEntity);
	void RenderAnimationsGUI(std::weak_ptr<World> weakWorld, ResourceManager& resourceManager,
	                         entt::entity selectedEntity);
	void RenderBonesOutline(std::weak_ptr<World> weakWorld, ResourceManager& resourceManager,
	                        RenderSystem& renderSystem, entt::entity selectedEntity);

	void RenderMaterialGUI(RenderSystem& renderSystem);

private:
	void _RenderModelBones(RenderSystem& renderSystem, Model& model, const ModelComponent& modelComp, int nodeIndex,
	                       const glm::mat4 parentTransform);
	void _ObjectTransform(TransformComponent& transform);

	entt::entity mPreviousEntity = entt::entity();
	bool mInitialized = false;

	// Bones
	int currentBoneSelection = 0;
	std::vector<std::string> boneKeys;
	std::vector<const char*> boneItems;

	// Animations
	int currentAnimationSelection = 0;
	std::vector<const char*> animItems;
};
