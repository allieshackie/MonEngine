#pragma once
#include <glm/mat4x4.hpp>

struct Animation;
struct AnimNode;
struct BoneNode;
struct MeshComponent;

class EntityRegistry;
class Model;
class ResourceManager;

class Animator
{
public:
	// Interpolate bone positions
	void Update(float timeStamp, EntityRegistry& entityRegistry, const ResourceManager& resourceManager);

private:
	void _UpdateBoneTransforms(float timeStamp, Animation* animation, Model& model, MeshComponent& mesh);
	void _ReadBoneHierarchy(float animationTime, Animation* animation, Model& model, MeshComponent& mesh,
	                        BoneNode* node, const glm::mat4 parentTransform);

	glm::mat4 _InterpolatePosition(double timeStamp, double ticksPerSecond, const AnimNode& bone);
	glm::mat4 _InterpolateRotation(double timeStamp, double ticksPerSecond, const AnimNode& bone);
	glm::mat4 _InterpolateScale(double timeStamp, double ticksPerSecond, const AnimNode& bone);

	int _GetPositionIndex(double timeStamp, const AnimNode& bone);
	int _GetRotationIndex(double timeStamp, const AnimNode& bone);
	int _GetScaleIndex(double timeStamp, const AnimNode& bone);

	const AnimNode* GetNodeAnim(const Animation* animation, const std::string& boneName);

	float mCurrentAnimationTime = 0;
};
