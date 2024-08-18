#pragma once

#include "Animation.h"

struct aiScene;
struct MeshComponent;

class EntityRegistry;
class ResourceManager;

class Animator
{
public:
	// Interpolate bone positions
	void Update(float timeStamp, EntityRegistry& entityRegistry, const ResourceManager& resourceManager);

private:
	void _UpdateBoneTransforms(float timeStamp, Animation& animation, MeshComponent& mesh);
	void _ReadBoneHierarchy(float animationTime, Animation& animation, aiNode* node,
	                        const glm::mat4 parentTransform);

	glm::mat4 _InterpolatePosition(double timeStamp, double ticksPerSecond, const BoneAnimNode& bone);
	glm::mat4 _InterpolateRotation(double timeStamp, double ticksPerSecond, const BoneAnimNode& bone);
	glm::mat4 _InterpolateScale(double timeStamp, double ticksPerSecond, const BoneAnimNode& bone);

	int _GetPositionIndex(double timeStamp, const BoneAnimNode& bone);
	int _GetRotationIndex(double timeStamp, const BoneAnimNode& bone);
	int _GetScaleIndex(double timeStamp, const BoneAnimNode& bone);
};
