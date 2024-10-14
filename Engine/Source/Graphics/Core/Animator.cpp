#include "Entity/EntityRegistry.h"
#include "Entity/Components/MapComponent.h"
#include "Entity/Components/MeshComponent.h"
#include "Util/gltfHelpers.h"
#include "ResourceManager.h"

#include "Animator.h"

Animator::Animator(EntityRegistry& entityRegistry, ResourceManager& resourceManager) : mResourceManager(resourceManager)
{
	entityRegistry.GetEnttRegistry().on_construct<MeshComponent>().connect<&Animator::_SetJointMatrixCount>(this);
}

void Animator::Update(float deltaTime, EntityRegistry& entityRegistry, const ResourceManager& resourceManager)
{
	const auto meshView = entityRegistry.GetEnttRegistry().view<MeshComponent>(entt::exclude<MapComponent>);

	meshView.each([this, &resourceManager, &deltaTime](MeshComponent& mesh)
	{
		if (!mesh.mCurrentAnimation.empty())
		{
			auto& model = resourceManager.GetModelFromId(mesh.mMeshPath);
			const auto animation = model.GetAnimation(mesh.mCurrentAnimation);
			if (animation != nullptr)
			{
				_UpdateAnimation(deltaTime, animation, model, mesh);
			}
		}
	});
}

void Animator::_UpdateAnimation(float deltaTime, const Animation* animation, Model& model, MeshComponent& mesh)
{
	mCurrentAnimationTime += deltaTime;
	if (mCurrentAnimationTime < animation->mStartFrameTime || mCurrentAnimationTime > animation->mEndFrameTime)
	{
		mCurrentAnimationTime = animation->mStartFrameTime;
	}

	_UpdateJointHierarchy(mCurrentAnimationTime, model, mesh, animation, model.GetRootNodeIndex(), glm::mat4(1.0f));
}

void Animator::_UpdateJointHierarchy(float animationTime, Model& model, MeshComponent& mesh, const Animation* animation,
                                     int nodeIndex, const glm::mat4 parentTransform)
{
	const auto jointNode = model.GetJointNodeAt(nodeIndex);

	if (jointNode == nullptr)
	{
		return;
	}

	glm::mat4 transform = jointNode->mTransformation;

	const auto nodeAnim = GetAnimNode(animation, nodeIndex);

	if (nodeAnim != nullptr)
	{
		transform =
			_InterpolatePosition(animationTime, nodeAnim) *
			_InterpolateRotation(animationTime, nodeAnim) *
			_InterpolateScale(animationTime, nodeAnim);
	}

	glm::mat4 globalTransform = parentTransform * transform;

	mesh.mFinalTransforms[nodeIndex] = globalTransform * jointNode->mInverseBindMatrix;

	for (const auto child : jointNode->mChildren)
	{
		_UpdateJointHierarchy(animationTime, model, mesh, animation, child, globalTransform);
	}
}

glm::mat4 Animator::_InterpolatePosition(float animationTime, const AnimNode* nodeData) const
{
	return glm::translate(glm::mat4(1.0f), _InterpolateVec3(animationTime, nodeData->mPositions));
}

glm::mat4 Animator::_InterpolateRotation(float animationTime, const AnimNode* nodeData) const
{
	return glm::toMat4(_InterpolateQuat(animationTime, nodeData->mRotations));
}

glm::mat4 Animator::_InterpolateScale(float animationTime, const AnimNode* nodeData) const
{
	return glm::scale(glm::mat4(1.0f), _InterpolateVec3(animationTime, nodeData->mScales));
}

glm::vec3 Animator::_InterpolateVec3(float animationTime, const std::vector<KeyframeVec>& keyframes) const
{
	if (keyframes.empty())
	{
		return glm::vec3(1.0f);
	}

	for (size_t i = 0; i < keyframes.size() - 1; ++i)
	{
		if (animationTime < keyframes[i + 1].mTimeStamp)
		{
			float t = (animationTime - keyframes[i].mTimeStamp) / (keyframes[i + 1].mTimeStamp - keyframes[i].
				mTimeStamp);
			return glm::mix(keyframes[i].mData, keyframes[i + 1].mData, t);
		}
	}

	return keyframes.back().mData;
}

glm::quat Animator::_InterpolateQuat(float animationTime, const std::vector<KeyframeQuat>& keyframes) const
{
	if (keyframes.empty())
	{
		return {1.0f, 0.0f, 0.0f, 0.0f};
	}

	for (size_t i = 0; i < keyframes.size() - 1; ++i)
	{
		if (animationTime < keyframes[i + 1].mTimeStamp)
		{
			float t = (animationTime - keyframes[i].mTimeStamp) / (keyframes[i + 1].mTimeStamp - keyframes[i].
				mTimeStamp);
			return glm::slerp(keyframes[i].mData, keyframes[i + 1].mData, t);
		}
	}

	return keyframes.back().mData;
}

const AnimNode* Animator::GetAnimNode(const Animation* animation, int nodeId)
{
	for (const auto animNode : animation->mAnimNodes)
	{
		if (animNode->mNodeIndex == nodeId)
		{
			return animNode;
		}
	}

	return nullptr;
}

void Animator::_SetJointMatrixCount(EnTTRegistry& registry, EntityId entity) const
{
	auto& mesh = registry.get<MeshComponent>(entity);
	const auto& model = mResourceManager.GetModelFromId(mesh.mMeshPath);

	if (mesh.mHasBones)
	{
		mesh.mFinalTransforms.resize(model.GetNumJoints());
	}
}
