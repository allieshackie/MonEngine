#include "assimp/scene.h"

#include "Entity/EntityRegistry.h"
#include "Entity/Components/MapComponent.h"
#include "Entity/Components/MeshComponent.h"
#include "Util/AssimpGLM.h"
#include "ResourceManager.h"

#include "Animator.h"

void Animator::Update(float timeStamp, EntityRegistry& entityRegistry, const ResourceManager& resourceManager)
{
	const auto meshView = entityRegistry.GetEnttRegistry().view<MeshComponent>(entt::exclude<MapComponent>);

	meshView.each([this, &resourceManager, &timeStamp](MeshComponent& mesh)
	{
		if (!mesh.mCurrentAnimation.empty())
		{
			auto& model = resourceManager.GetModelFromId(mesh.mMeshPath);
			auto& animation = model.GetAnimation(mesh.mCurrentAnimation);
			_UpdateBoneTransforms(timeStamp, animation, mesh);
		}
	});
}

void Animator::_UpdateBoneTransforms(float timeStamp, Animation& animation, MeshComponent& mesh)
{
	float timeInTicks = (timeStamp / 1000.0f) * static_cast<float>(animation.mTicksPerSecond);
	float animationTime = fmod(timeInTicks, animation.mDuration);

	auto identityMat = glm::mat4(1.0f);
	_ReadBoneHierarchy(animationTime, animation, animation.mRootNode, identityMat);
}

void Animator::_ReadBoneHierarchy(float animationTime, Animation& animation, aiNode* node,
                                  const glm::mat4 parentTransform)
{
	/*
	 *
	bone.mLocalTransform =
		_InterpolatePosition(animationTime, animation.mTicksPerSecond, bone) *
		_InterpolateRotation(animationTime, animation.mTicksPerSecond, bone) *
		_InterpolateScale(animationTime, animation.mTicksPerSecond, bone);

	glm::mat4 globalTransformation = parentTransform * nodeTransform;
	 */
}

glm::mat4 Animator::_InterpolatePosition(double timeStamp, double ticksPerSecond, const BoneAnimNode& bone)
{
	if (bone.mNumPositions == 1)
	{
		return glm::translate(glm::mat4(1.0f), bone.mPositions[0].mData);
	}

	int positionIndex = _GetPositionIndex(timeStamp, bone);
	int nextPositionIndex = positionIndex + 1;

	double t1 = bone.mPositions[positionIndex].mTimeStamp;
	double t2 = bone.mPositions[nextPositionIndex].mTimeStamp;

	double deltaTime = t2 - t1;
	auto factor = static_cast<float>((ticksPerSecond - t1) / deltaTime);

	glm::vec3 posDelta = bone.mPositions[nextPositionIndex].mData - bone.mPositions[positionIndex].mData;
	return glm::translate(glm::mat4(1.0f), bone.mPositions[positionIndex].mData + factor * posDelta);
}

glm::mat4 Animator::_InterpolateRotation(double timeStamp, double ticksPerSecond, const BoneAnimNode& bone)
{
	if (bone.mNumRotations == 1)
	{
		return glm::mat4(bone.mRotations[0].mData);
	}

	int rotationIndex = _GetRotationIndex(timeStamp, bone);
	int nextRotationIndex = rotationIndex + 1;

	double t1 = bone.mRotations[rotationIndex].mTimeStamp;
	double t2 = bone.mRotations[nextRotationIndex].mTimeStamp;

	double deltaTime = t2 - t1;
	auto factor = static_cast<float>((ticksPerSecond - t1) / deltaTime);

	auto result = glm::slerp(bone.mRotations[rotationIndex].mData, bone.mRotations[nextRotationIndex].mData, factor);
	return glm::mat4(glm::normalize(result));
}

glm::mat4 Animator::_InterpolateScale(double timeStamp, double ticksPerSecond, const BoneAnimNode& bone)
{
	if (bone.mNumScales == 1)
	{
		return glm::scale(glm::mat4(1.0f), bone.mScales[0].mData);
	}

	int scaleIndex = _GetScaleIndex(timeStamp, bone);
	int nextScaleIndex = scaleIndex + 1;

	double t1 = bone.mScales[scaleIndex].mTimeStamp;
	double t2 = bone.mScales[nextScaleIndex].mTimeStamp;

	double deltaTime = t2 - t1;
	auto factor = static_cast<float>((ticksPerSecond - t1) / deltaTime);

	glm::vec3 scaleDelta = bone.mScales[nextScaleIndex].mData - bone.mScales[scaleIndex].mData;
	return glm::scale(glm::mat4(1.0f), bone.mScales[scaleIndex].mData + factor * scaleDelta);
}

int Animator::_GetPositionIndex(double timeStamp, const BoneAnimNode& bone)
{
	for (int i = 0; i < bone.mNumPositions - 1; i++)
	{
		if (timeStamp < bone.mPositions[i + 1].mTimeStamp)
		{
			return i;
		}
	}

	return 0;
}

int Animator::_GetRotationIndex(double timeStamp, const BoneAnimNode& bone)
{
	for (int i = 0; i < bone.mNumRotations - 1; i++)
	{
		if (timeStamp < bone.mRotations[i + 1].mTimeStamp)
		{
			return i;
		}
	}

	return 0;
}

int Animator::_GetScaleIndex(double timeStamp, const BoneAnimNode& bone)
{
	for (int i = 0; i < bone.mNumScales - 1; i++)
	{
		if (timeStamp < bone.mScales[i + 1].mTimeStamp)
		{
			return i;
		}
	}

	return 0;
}
