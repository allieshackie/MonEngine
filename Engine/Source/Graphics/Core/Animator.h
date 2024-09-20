#pragma once
#include <glm/mat4x4.hpp>

struct aiAnimation;
struct aiNodeAnim;

struct BoneNode;
struct MeshComponent;

class EntityRegistry;
class Model;
class ResourceManager;

class Animator
{
public:
	// Interpolate bone positions
	void Update(float deltaTime, EntityRegistry& entityRegistry, const ResourceManager& resourceManager);

private:
	void _UpdateBoneTransforms(float deltaTime, aiAnimation* animation, Model& model, MeshComponent& mesh);
	void _ReadBoneHierarchy(float animationTime, aiAnimation* animation, Model& model, MeshComponent& mesh,
	                        BoneNode* node, const glm::mat4 parentTransform);

	glm::mat4 _InterpolatePosition(double timeStamp, const aiNodeAnim& bone);
	glm::mat4 _InterpolateRotation(double timeStamp, const aiNodeAnim& bone);
	glm::mat4 _InterpolateScale(double timeStamp, const aiNodeAnim& bone);

	int _GetPositionIndex(double timeStamp, const aiNodeAnim& bone);
	int _GetRotationIndex(double timeStamp, const aiNodeAnim& bone);
	int _GetScaleIndex(double timeStamp, const aiNodeAnim& bone);

	const aiNodeAnim* GetNodeAnim(const aiAnimation* animation, const std::string& boneName);

	float mCurrentAnimationTime = 0;
};
