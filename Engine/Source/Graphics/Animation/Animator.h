#pragma once
#include <glm/mat4x4.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Entity/Components/AnimationComponent.h"

#include "Animation.h"
#include "Core/ISystem.h"

struct ModelComponent;

class Entity;
class Model;
class ResourceManager;
class World;

class AnimatorSystem : public ISystem
{
public:
	AnimatorSystem(ResourceManager& resourceManager, std::weak_ptr<World> world);

	void Update(float dt) override;

	void SetSceneCallbacks() const;

private:
	void _UpdateAnimation(float deltaTime, Model& model, AnimationComponent& animComp, ModelComponent& mesh);
	void _UpdateBlend(float deltaTime, AnimationComponent& animComp);
	void _UpdateJointHierarchy(Model& model, AnimationComponent& animComp, ModelComponent& mesh,
	                           const Animation* animation, const Animation* prevAnimation, int nodeIndex,
	                           const glm::mat4 parentTransform);

	glm::mat4 _InterpolatePosition(float animationTime, const AnimNode* nodeData) const;
	glm::mat4 _InterpolateRotation(float animationTime, const AnimNode* nodeData) const;
	glm::mat4 _InterpolateScale(float animationTime, const AnimNode* nodeData) const;

	glm::vec3 _InterpolateVec3(float animationTime, const std::vector<KeyframeVec>& keyframes) const;
	glm::quat _InterpolateQuat(float animationTime, const std::vector<KeyframeQuat>& keyframes) const;

	glm::mat4 _InterpolateMatrices(const glm::mat4& matA, const glm::mat4& matB, float blendFactor) const;

	void _ApplyBlendTime(const AnimationComponent& animComp);
	void _SetJointMatrixCount(Entity* entity) const;

	const AnimNode* GetAnimNode(const Animation* animation, int nodeId);

	float mPrevAnimationTime = 0.0f;
	float mCurrentAnimationTime = 0.0f;

	std::weak_ptr<World> mWorld;
	ResourceManager& mResourceManager;
};
