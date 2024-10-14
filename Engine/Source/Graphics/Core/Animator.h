#pragma once
#include <glm/mat4x4.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Animation.h"

struct MeshComponent;

class EntityRegistry;
class Model;
class ResourceManager;

class Animator
{
public:
	Animator(EntityRegistry& entityRegistry, ResourceManager& resourceManager);
	// Interpolate bone positions
	void Update(float deltaTime, EntityRegistry& entityRegistry, const ResourceManager& resourceManager);

private:
	void _UpdateAnimation(float deltaTime, const Animation* animation, Model& model, MeshComponent& mesh);
	void _UpdateJointHierarchy(float animationTime, Model& model, MeshComponent& mesh, const Animation* animation,
	                           int nodeIndex, const glm::mat4 parentTransform);

	glm::mat4 _InterpolatePosition(float animationTime, const AnimNode* nodeData) const;
	glm::mat4 _InterpolateRotation(float animationTime, const AnimNode* nodeData) const;
	glm::mat4 _InterpolateScale(float animationTime, const AnimNode* nodeData) const;

	glm::vec3 _InterpolateVec3(float animationTime, const std::vector<KeyframeVec>& keyframes) const;
	glm::quat _InterpolateQuat(float animationTime, const std::vector<KeyframeQuat>& keyframes) const;

	void _SetJointMatrixCount(EnTTRegistry& registry, EntityId entity) const;

	const AnimNode* GetAnimNode(const Animation* animation, int nodeId);

	float mCurrentAnimationTime = 0;

	ResourceManager& mResourceManager;
};
