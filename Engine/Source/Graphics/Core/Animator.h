#pragma once
#include <glm/mat4x4.hpp>

struct aiAnimation;
struct aiScene;
struct aiNodeAnim;
struct aiNode;
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
	void _UpdateBoneTransforms(float timeStamp, aiAnimation* animation, Model& model, MeshComponent& mesh);
	void _ReadBoneHierarchy(float animationTime, aiAnimation* animation, Model& model, MeshComponent& mesh,
	                        aiNode* node, const glm::mat4 parentTransform);

	glm::mat4 _InterpolatePosition(double timeStamp, double ticksPerSecond, const aiNodeAnim& bone);
	glm::mat4 _InterpolateRotation(double timeStamp, double ticksPerSecond, const aiNodeAnim& bone);
	glm::mat4 _InterpolateScale(double timeStamp, double ticksPerSecond, const aiNodeAnim& bone);

	int _GetPositionIndex(double timeStamp, const aiNodeAnim& bone);
	int _GetRotationIndex(double timeStamp, const aiNodeAnim& bone);
	int _GetScaleIndex(double timeStamp, const aiNodeAnim& bone);

	const aiNodeAnim* GetNodeAnim(const aiAnimation* animation, const std::string& boneName);
};
