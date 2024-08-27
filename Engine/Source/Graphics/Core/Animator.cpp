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
			auto animation = model.GetAnimation(mesh.mCurrentAnimation);
			if (animation->mDuration != 0.0)
			{
				_UpdateBoneTransforms(timeStamp, animation, model, mesh);
			}
		}
	});
}

void Animator::_UpdateBoneTransforms(float timeStamp, Animation* animation, Model& model, MeshComponent& mesh)
{
	float timeInTicks = timeStamp * static_cast<float>(animation->mTicksPerSecond);
	mCurrentAnimationTime += timeInTicks;
	float animationTime = fmod(mCurrentAnimationTime, animation->mDuration);

	auto identityMat = glm::mat4(1.0f);
	_ReadBoneHierarchy(animationTime, animation, model, mesh, model.GetRootNode(), identityMat);
}

void Animator::_ReadBoneHierarchy(float animationTime, Animation* animation, Model& model, MeshComponent& mesh,
                                  BoneNode* node, const glm::mat4 parentTransform)
{
	// TODO: Move this somewhere that makes sense
	if (mesh.mFinalTransforms.empty())
	{
		mesh.mFinalTransforms.resize(model.GetBoneCount());
	}

	glm::mat4 transform = node->mTransformation;

	const auto nodeAnim = GetNodeAnim(animation, node->mId);

	if (nodeAnim != nullptr)
	{
		transform = _InterpolatePosition(animationTime, animation->mTicksPerSecond, *nodeAnim) *
			_InterpolateRotation(animationTime, animation->mTicksPerSecond, *nodeAnim) *
			_InterpolateScale(animationTime, animation->mTicksPerSecond, *nodeAnim);
	}

	glm::mat4 globalTransform = parentTransform * transform;

	if (int boneIndex = model.GetBoneIndex(node->mId); boneIndex != -1)
	{
		mesh.mFinalTransforms[boneIndex] = model.GetRootInverseTransform() * globalTransform * model.
			GetBoneOffset(boneIndex);
	}

	for (const auto child : node->mChildren)
	{
		_ReadBoneHierarchy(animationTime, animation, model, mesh, child, globalTransform);
	}
}

glm::mat4 Animator::_InterpolatePosition(double timeStamp, double ticksPerSecond, const AnimNode& bone)
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
	auto factor = static_cast<float>((timeStamp - t1) / deltaTime);

	auto firstPos = bone.mPositions[positionIndex].mData;
	auto nextPos = bone.mPositions[nextPositionIndex].mData;
	glm::vec3 posDelta = nextPos - firstPos;
	return glm::translate(glm::mat4(1.0f), firstPos + factor * posDelta);
}

glm::mat4 Animator::_InterpolateRotation(double timeStamp, double ticksPerSecond, const AnimNode& bone)
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
	auto factor = static_cast<float>((timeStamp - t1) / deltaTime);

	auto firstRot = bone.mRotations[rotationIndex].mData;
	auto nextRot = bone.mRotations[nextRotationIndex].mData;
	auto result = glm::slerp(firstRot, nextRot, factor);
	return glm::mat4(glm::normalize(result));
}

glm::mat4 Animator::_InterpolateScale(double timeStamp, double ticksPerSecond, const AnimNode& bone)
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
	auto factor = static_cast<float>((timeStamp - t1) / deltaTime);

	auto firstScale = bone.mScales[scaleIndex].mData;
	auto nextScale = bone.mScales[nextScaleIndex].mData;
	glm::vec3 scaleDelta = nextScale - firstScale;
	return glm::scale(glm::mat4(1.0f), firstScale + factor * scaleDelta);
}

int Animator::_GetPositionIndex(double timeStamp, const AnimNode& bone)
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

int Animator::_GetRotationIndex(double timeStamp, const AnimNode& bone)
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

int Animator::_GetScaleIndex(double timeStamp, const AnimNode& bone)
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

const AnimNode* Animator::GetNodeAnim(const Animation* animation, const std::string& boneName)
{
	for (const auto animNode : animation->mBoneAnimNodes)
	{
		if (animNode->mName == boneName)
		{
			return animNode;
		}
	}

	return nullptr;
}
