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

void Animator::_UpdateBoneTransforms(float timeStamp, aiAnimation* animation, Model& model, MeshComponent& mesh)
{
	float timeInTicks = (timeStamp / 1000.0f) * static_cast<float>(animation->mTicksPerSecond);
	float animationTime = fmod(timeInTicks, animation->mDuration);

	auto identityMat = glm::mat4(1.0f);
	_ReadBoneHierarchy(animationTime, animation, model, mesh, model.GetRootNode(), identityMat);
}

void Animator::_ReadBoneHierarchy(float animationTime, aiAnimation* animation, Model& model, MeshComponent& mesh,
                                  aiNode* node, const glm::mat4 parentTransform)
{
	glm::mat4 transform = AssimpGLM::ConvertMatrix(node->mTransformation);

	const auto nodeAnim = GetNodeAnim(animation, node->mName.C_Str());

	if (nodeAnim != nullptr)
	{
		transform = _InterpolatePosition(animationTime, animation->mTicksPerSecond, *nodeAnim) *
			_InterpolateRotation(animationTime, animation->mTicksPerSecond, *nodeAnim) *
			_InterpolateScale(animationTime, animation->mTicksPerSecond, *nodeAnim);
	}

	glm::mat4 globalTransform = parentTransform * transform;

	int boneIndex = model.GetBoneIndex(node->mName.C_Str());
	mesh.mFinalTransforms[boneIndex] = model.GetRootInverseTransform() * globalTransform * model.
		GetBoneOffset(boneIndex);


	for (int i = 0; i < node->mNumChildren; i++)
	{
		_ReadBoneHierarchy(animationTime, animation, model, mesh, node->mChildren[i], globalTransform);
	}
}

glm::mat4 Animator::_InterpolatePosition(double timeStamp, double ticksPerSecond, const aiNodeAnim& bone)
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
	auto factor = static_cast<float>((ticksPerSecond - t1) / deltaTime);

	auto firstPos = AssimpGLM::ConvertVec(bone.mPositionKeys[positionIndex].mValue);
	auto nextPos = AssimpGLM::ConvertVec(bone.mPositionKeys[nextPositionIndex].mValue);
	glm::vec3 posDelta = nextPos - firstPos;
	return glm::translate(glm::mat4(1.0f), firstPos + factor * posDelta);
}

glm::mat4 Animator::_InterpolateRotation(double timeStamp, double ticksPerSecond, const aiNodeAnim& bone)
{
	if (bone.mNumRotationKeys == 1)
	{
		return glm::mat4(AssimpGLM::ConvertQuat(bone.mRotationKeys[0].mValue));
	}

	int rotationIndex = _GetRotationIndex(timeStamp, bone);
	int nextRotationIndex = rotationIndex + 1;

	double t1 = bone.mRotationKeys[rotationIndex].mTime;
	double t2 = bone.mRotationKeys[nextRotationIndex].mTime;

	double deltaTime = t2 - t1;
	auto factor = static_cast<float>((ticksPerSecond - t1) / deltaTime);

	auto firstRot = AssimpGLM::ConvertQuat(bone.mRotationKeys[rotationIndex].mValue);
	auto nextRot = AssimpGLM::ConvertQuat(bone.mRotationKeys[nextRotationIndex].mValue);
	auto result = glm::slerp(firstRot, nextRot, factor);
	return glm::mat4(glm::normalize(result));
}

glm::mat4 Animator::_InterpolateScale(double timeStamp, double ticksPerSecond, const aiNodeAnim& bone)
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
	auto factor = static_cast<float>((ticksPerSecond - t1) / deltaTime);

	auto firstScale = AssimpGLM::ConvertVec(bone.mScalingKeys[scaleIndex].mValue);
	auto nextScale = AssimpGLM::ConvertVec(bone.mScalingKeys[nextScaleIndex].mValue);
	glm::vec3 scaleDelta = nextScale - firstScale;
	return glm::scale(glm::mat4(1.0f), firstScale + factor * scaleDelta);
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
		if (animation->mChannels[i]->mNodeName.data == boneName)
		{
			return animation->mChannels[i];
		}
	}

	return nullptr;
}
