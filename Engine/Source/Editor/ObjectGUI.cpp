#include <imgui.h>
#include "Core/World.h"
#include "Entity/Components/AnimationComponent.h"
#include "Entity/Components/ModelComponent.h"
#include "Entity/Components/TransformComponent.h"
#include "Graphics/Core/Node.h"
#include "Graphics/Core/ResourceManager.h"
#include "Graphics/RenderSystem.h"

#include "ObjectGUI.h"

void ObjectGUI::RenderBonesGUI(std::weak_ptr<World> weakWorld, ResourceManager& resourceManager,
                               entt::entity selectedEntity)
{
	const auto world = weakWorld.lock();
	if (!(world && world->GetRegistry().valid(selectedEntity)))
	{
		return;
	}

	auto mesh = world->GetRegistry().try_get<ModelComponent>(selectedEntity);
	if (mesh == nullptr)
	{
		return;
	}

	const auto model = resourceManager.GetModelFromId(mesh->mModelPath);
	if (model.GetNumJoints() == 0)
	{
		return;
	}

	if (mPreviousEntity != selectedEntity || !mInitialized)
	{
		mInitialized = true;
		mPreviousEntity = selectedEntity;
		boneKeys.clear();
		boneItems.clear();
		animItems.clear();

		auto& boneNamesToIndex = model.GetBoneNamesToIndex();
		for (const auto& kv : boneNamesToIndex)
		{
			boneKeys.push_back(kv.first);
		}

		for (const std::string& key : boneKeys)
		{
			boneItems.push_back(key.c_str());
		}

		const auto& animations = model.GetAllAnimations();
		for (const auto& animation : animations)
		{
			animItems.push_back(animation->mName.c_str());
		}
	}

	if (ImGui::Combo("Bones", &currentBoneSelection, boneItems.data(), boneItems.size()))
	{
		auto& boneNamesToIndex = model.GetBoneNamesToIndex();
		mesh->mCurrentBoneIndex = boneNamesToIndex.find(boneKeys[currentBoneSelection])->second;
	}
}

void ObjectGUI::RenderAnimationsGUI(std::weak_ptr<World> weakWorld, ResourceManager& resourceManager,
                                    entt::entity selectedEntity)
{
	auto world = weakWorld.lock();
	if (!(world && world->GetRegistry().valid(selectedEntity)))
	{
		return;
	}
	if (ImGui::Combo("Animations", &currentAnimationSelection, animItems.data(), animItems.size()))
	{
		auto animComponent = world->GetRegistry().try_get<AnimationComponent>(selectedEntity);
		if (animComponent)
		{
			animComponent->mCurrentAnimState = currentAnimationSelection;
		}
	}
}

void ObjectGUI::RenderBonesOutline(std::weak_ptr<World> weakWorld, ResourceManager& resourceManager,
                                   RenderSystem& renderSystem, entt::entity selectedEntity)
{
	const auto world = weakWorld.lock();
	if (!(world && world->GetRegistry().valid(selectedEntity)))
	{
		return;
	}

	const auto modelComponent = world->GetRegistry().try_get<ModelComponent>(selectedEntity);
	const auto transformComponent = world->GetRegistry().try_get<TransformComponent>(selectedEntity);
	if (modelComponent == nullptr || transformComponent == nullptr)
	{
		return;
	}

	auto model = resourceManager.GetModelFromId(modelComponent->mModelPath);
	_RenderModelBones(renderSystem, model, *modelComponent, model.GetRootJointIndex(), glm::mat4(1.0f));
}

void ObjectGUI::_RenderModelBones(RenderSystem& renderSystem, Model& model, const ModelComponent& modelComp,
                                  int nodeIndex,
                                  const glm::mat4 parentTransform)
{
	const auto node = model.GetNodeAt(nodeIndex);

	if (node == nullptr)
	{
		return;
	}

	const auto jointNode = model.GetJointDataAt(node->mJointIndex);
	if (jointNode == nullptr)
	{
		return;
	}

	glm::mat4 globalTransform = parentTransform * node->mTransform;
	auto bonePosition = glm::vec3(globalTransform[3]);
	auto parentPosition = glm::vec3(parentTransform[3]);

	// Default length for visualization purposes
	float defaultLength = 1.0f; // Adjust this as needed
	glm::vec3 direction = glm::normalize(bonePosition - parentPosition) * defaultLength;
	glm::vec3 endPosition = bonePosition + direction;

	glm::vec4 color = {1, 1, 1, 1};
	if (strcmp(jointNode->mId.c_str(), "Head") == 0)
	{
		color = {1, 0, 0, 1};
	}

	renderSystem.DrawLine(bonePosition, endPosition, color);

	for (const auto child : node->mChildren)
	{
		_RenderModelBones(renderSystem, model, modelComp, child, globalTransform);
	}
}

void ObjectGUI::_ObjectTransform(TransformComponent& transform)
{
	const auto rot = transform.mRotation;
	float tempPosition[3] = {rot.x, rot.y, rot.z};
	if (ImGui::InputFloat3("Rotation", tempPosition))
	{
		transform.mRotation = {tempPosition[0], tempPosition[1], tempPosition[2]};
	}
}
