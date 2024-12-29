#pragma once
#include <glm/mat4x4.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Entity/Components/AnimationComponent.h"

#include "Animation.h"

struct MeshComponent;

class Entity;
class Model;
class MonScene;
class ResourceManager;

class Animator
{
public:
	Animator(ResourceManager& resourceManager);
	// Interpolate bone positions
	void Update(float deltaTime, MonScene* scene, const ResourceManager& resourceManager);

	void SetSceneCallbacks(const MonScene* scene) const;

private:
	void _UpdateAnimation(float deltaTime, Model& model, AnimationComponent& animComp, MeshComponent& mesh);
	void _UpdateBlend(float deltaTime, AnimationComponent& animComp);
	void _UpdateJointHierarchy(Model& model, AnimationComponent& animComp, MeshComponent& mesh,
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

	ResourceManager& mResourceManager;
};
