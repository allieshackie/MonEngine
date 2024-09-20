#include <glm/gtx/quaternion.hpp>
#include "assimp/scene.h"

#include "Entity/EntityRegistry.h"
#include "Entity/Components/MapComponent.h"
#include "Entity/Components/MeshComponent.h"
#include "Util/AssimpGLM.h"
#include "ResourceManager.h"

#include "Animator.h"

void Animator::Update(float deltaTime, EntityRegistry& entityRegistry, const ResourceManager& resourceManager)
{
	const auto meshView = entityRegistry.GetEnttRegistry().view<MeshComponent>(entt::exclude<MapComponent>);

	meshView.each([this, &resourceManager, &deltaTime](MeshComponent& mesh)
	{
		if (!mesh.mCurrentAnimation.empty())
		{
			auto& model = resourceManager.GetModelFromId(mesh.mMeshPath);
			auto animation = model.GetAnimation(mesh.mCurrentAnimation);
			if (animation->mDuration != 0.0)
			{
				_UpdateBoneTransforms(deltaTime, animation, model, mesh);
			}
		}
	});
}

void Animator::_UpdateBoneTransforms(float deltaTime, aiAnimation* animation, Model& model, MeshComponent& mesh)
{
	float timeInTicks = deltaTime * static_cast<float>(animation->mTicksPerSecond);
	mCurrentAnimationTime += timeInTicks;
	float animationTime = fmod(mCurrentAnimationTime, animation->mDuration);

	auto identityMat = glm::mat4(1.0f);
	_ReadBoneHierarchy(animationTime, animation, model, mesh, model.GetRootNode(), identityMat);
}

void Animator::_ReadBoneHierarchy(float animationTime, aiAnimation* animation, Model& model, MeshComponent& mesh,
                                  BoneNode* node, const glm::mat4 parentTransform)
{
	if (std::string(node->mId.c_str()).find("_$AssimpFbx$_") != std::string::npos)
	{
		for (const auto child : node->mChildren)
		{
			_ReadBoneHierarchy(animationTime, animation, model, mesh, child, parentTransform);
		}
		return;
	}

	glm::mat4 transform = node->mTransformation;
	const auto nodeAnim = GetNodeAnim(animation, node->mId);

	if (nodeAnim != nullptr)
	{
		transform =
			_InterpolatePosition(animationTime, *nodeAnim) *
			_InterpolateRotation(animationTime, *nodeAnim) *
			_InterpolateScale(animationTime, *nodeAnim);
	}

	glm::mat4 globalTransform = parentTransform * transform;

	if (int boneIndex = model.GetBoneIndex(node->mId); boneIndex != -1)
	{
		model.GetBoneInfos()[boneIndex]->mFinalTransform = model.GetRootInverseTransform() * globalTransform * model.
			GetBoneOffset(boneIndex);
	}

	for (const auto child : node->mChildren)
	{
		_ReadBoneHierarchy(animationTime, animation, model, mesh, child, globalTransform);
	}
}

glm::mat4 Animator::_InterpolatePosition(double timeStamp, const aiNodeAnim& bone)
{
	if (bone.mNumPositionKeys == 1)
	{
		return glm::translate(glm::mat4(1.0f), AssimpGLM::ConvertVec(bone.mPositionKeys[0].mValue));
	}

	int positionIndex = _GetPositionIndex(timeStamp, bone);
	int nextPositionIndex = positionIndex + 1;

	double t1 = bone.mPositionKeys[positionIndex].mTime;
	double t2 = bone.mPositionKeys[nextPositionIndex].mTime;

	double deltaTime = t2 - t1;
	auto factor = static_cast<float>((timeStamp - t1) / deltaTime);

	const auto& firstPos = bone.mPositionKeys[positionIndex].mValue;
	const auto& nextPos = bone.mPositionKeys[nextPositionIndex].mValue;
	glm::vec3 finalPosition = glm::mix(AssimpGLM::ConvertVec(firstPos), AssimpGLM::ConvertVec(nextPos), factor);
	return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 Animator::_InterpolateRotation(double timeStamp, const aiNodeAnim& bone)
{
	if (bone.mNumRotationKeys == 1)
	{
		return glm::toMat4(glm::normalize(AssimpGLM::ConvertQuat(bone.mRotationKeys[0].mValue)));
	}

	int rotationIndex = _GetRotationIndex(timeStamp, bone);
	int nextRotationIndex = rotationIndex + 1;

	double t1 = bone.mRotationKeys[rotationIndex].mTime;
	double t2 = bone.mRotationKeys[nextRotationIndex].mTime;

	double deltaTime = t2 - t1;
	auto factor = static_cast<float>((timeStamp - t1) / deltaTime);

	glm::quat finalRotation = glm::slerp(AssimpGLM::ConvertQuat(bone.mRotationKeys[rotationIndex].mValue),
	                                     AssimpGLM::ConvertQuat(bone.mRotationKeys[nextRotationIndex].mValue), factor);
	finalRotation = glm::normalize(finalRotation);
	return glm::toMat4(finalRotation);
}

glm::mat4 Animator::_InterpolateScale(double timeStamp, const aiNodeAnim& bone)
{
	if (bone.mNumScalingKeys == 1)
	{
		return glm::scale(glm::mat4(1.0f), AssimpGLM::ConvertVec(bone.mScalingKeys[0].mValue));
	}

	int scaleIndex = _GetScaleIndex(timeStamp, bone);
	int nextScaleIndex = scaleIndex + 1;

	double t1 = bone.mScalingKeys[scaleIndex].mTime;
	double t2 = bone.mScalingKeys[nextScaleIndex].mTime;

	double deltaTime = t2 - t1;
	auto factor = static_cast<float>((timeStamp - t1) / deltaTime);

	const auto& firstScale = bone.mScalingKeys[scaleIndex].mValue;
	const auto& nextScale = bone.mScalingKeys[nextScaleIndex].mValue;
	glm::vec3 finalScale = glm::mix(AssimpGLM::ConvertVec(firstScale), AssimpGLM::ConvertVec(nextScale), factor);
	return glm::scale(glm::mat4(1.0f), finalScale);
}

int Animator::_GetPositionIndex(double timeStamp, const aiNodeAnim& bone)
{
	for (int i = 0; i < bone.mNumPositionKeys - 1; i++)
	{
		if (timeStamp < bone.mPositionKeys[i + 1].mTime)
		{
			return i;
		}
	}

	return 0;
}

int Animator::_GetRotationIndex(double timeStamp, const aiNodeAnim& bone)
{
	for (int i = 0; i < bone.mNumRotationKeys - 1; i++)
	{
		if (timeStamp < bone.mRotationKeys[i + 1].mTime)
		{
			return i;
		}
	}

	return 0;
}

int Animator::_GetScaleIndex(double timeStamp, const aiNodeAnim& bone)
{
	for (int i = 0; i < bone.mNumScalingKeys - 1; i++)
	{
		if (timeStamp < bone.mScalingKeys[i + 1].mTime)
		{
			return i;
		}
	}

	return 0;
}

const aiNodeAnim* Animator::GetNodeAnim(const aiAnimation* animation, const std::string& boneName)
{
	for (int i = 0; i < animation->mNumChannels; i++)
	{
		const aiNodeAnim* pNodeAnim = animation->mChannels[i];

		if (pNodeAnim->mNodeName.C_Str() == boneName)
		{
			return pNodeAnim;
		}
	}

	return nullptr;
}
