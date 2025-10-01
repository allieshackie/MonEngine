#include "Core/World.h"
#include "Entity/Entity.h"
#include "Entity/Components/AnimationComponent.h"
#include "Entity/Components/ModelComponent.h"
#include "Graphics/Core/ResourceManager.h"
#include "Graphics/Core/Node.h"
#include "Util/gltfHelpers.h"

#include "Animator.h"

AnimatorSystem::AnimatorSystem(ResourceManager& resourceManager, std::weak_ptr<World> world) :
	mWorld(std::move(world)), mResourceManager(resourceManager)
{
}

void AnimatorSystem::Update(float dt)
{
	if (const auto world = mWorld.lock())
	{
		const auto meshView = world->GetRegistry().view<AnimationComponent, ModelComponent>();

		meshView.each([this, &dt](AnimationComponent& anim, ModelComponent& mesh)
		{
			auto& model = mResourceManager.GetModelFromId(mesh.mModelPath);
			_UpdateAnimation(dt, model, anim, mesh);
		});
	}
}

void AnimatorSystem::SetSceneCallbacks() const
{
	if (const auto world = mWorld.lock())
	{
		EventFunc func = [this](Entity* entity)
		{
			_SetJointMatrixCount(entity);
		};
		world->ConnectOnConstruct<ModelComponent>(func);
	}
}

void AnimatorSystem::_UpdateAnimation(float deltaTime, Model& model, AnimationComponent& animComp, ModelComponent& mesh)
{
	const auto animation = model.GetAnimation(animComp.mCurrentAnimState);
	const auto prevAnimation = model.GetAnimation(animComp.mPrevAnimState);

	mCurrentAnimationTime += deltaTime;
	if (animComp.mUpdated)
	{
		_ApplyBlendTime(animComp);
		animComp.mUpdated = false;
	}
	_UpdateBlend(deltaTime, animComp);

	if (animation != nullptr)
	{
		if (mCurrentAnimationTime < animation->mStartFrameTime || mCurrentAnimationTime > animation->mEndFrameTime)
		{
			mCurrentAnimationTime = animation->mStartFrameTime;
		}
	}

	if (prevAnimation != nullptr)
	{
		if (mPrevAnimationTime < prevAnimation->mStartFrameTime || mPrevAnimationTime > prevAnimation->mEndFrameTime)
		{
			mPrevAnimationTime = prevAnimation->mStartFrameTime;
		}
	}

	_UpdateJointHierarchy(model, animComp, mesh, animation, prevAnimation, model.GetRootNodeIndex(),
	                      glm::mat4(1.0f));
}

void AnimatorSystem::_UpdateBlend(float deltaTime, AnimationComponent& animComp)
{
	if (animComp.mBlendFactor < 1.0f)
	{
		float transitionTime = 1.0f;
		for (const auto& transition : animComp.mTransitions)
		{
			if (transition.mTransitionFrom == animComp.mPrevAnimState && transition.mTransitionTo == animComp.
				mCurrentAnimState)
			{
				transitionTime = transition.mTransitionTime;
			}
		}

		// Update the blend factor over a fixed duration, regardless of animation length
		animComp.mBlendFactor += deltaTime / transitionTime;
		animComp.mBlendFactor = std::min(animComp.mBlendFactor, 1.0f); // Clamp to 1.0 when blend is done

		if (animComp.mBlendFactor >= 1.0f)
		{
			animComp.mPrevAnimState = -1;
			mPrevAnimationTime = 0.0f;
		}
	}
}

void AnimatorSystem::_UpdateJointHierarchy(Model& model, AnimationComponent& animComp, ModelComponent& mesh,
                                           const Animation* animation, const Animation* prevAnimation, int nodeIndex,
                                           const glm::mat4 parentTransform)
{
	const auto node = model.GetNodeAt(nodeIndex);
	const auto jointNode = model.GetJointDataAt(node->mJointIndex);

	if (node == nullptr || jointNode == nullptr)
	{
		return;
	}

	glm::mat4 transform = node->mTransform;

	if (animation != nullptr)
	{
		const auto nodeAnim = GetAnimNode(animation, nodeIndex);

		if (nodeAnim != nullptr)
		{
			transform =
				_InterpolatePosition(mCurrentAnimationTime, nodeAnim) *
				_InterpolateRotation(mCurrentAnimationTime, nodeAnim) *
				_InterpolateScale(mCurrentAnimationTime, nodeAnim);
		}

		if (prevAnimation != nullptr)
		{
			const auto prevNodeAnim = GetAnimNode(prevAnimation, nodeIndex);

			if (prevNodeAnim != nullptr)
			{
				glm::mat4 prevTransform =
					_InterpolatePosition(mPrevAnimationTime, nodeAnim) *
					_InterpolateRotation(mPrevAnimationTime, nodeAnim) *
					_InterpolateScale(mPrevAnimationTime, nodeAnim);

				transform = _InterpolateMatrices(prevTransform, transform, animComp.mBlendFactor);
			}
		}
	}

	glm::mat4 globalTransform = parentTransform * transform;

	mesh.mFinalTransforms[nodeIndex] = globalTransform * jointNode->mInverseBindMatrix;

	for (const auto child : node->mChildren)
	{
		_UpdateJointHierarchy(model, animComp, mesh, animation, prevAnimation, child, globalTransform);
	}
}

glm::mat4 AnimatorSystem::_InterpolatePosition(float animationTime, const AnimNode* nodeData) const
{
	return glm::translate(glm::mat4(1.0f), _InterpolateVec3(animationTime, nodeData->mPositions));
}

glm::mat4 AnimatorSystem::_InterpolateRotation(float animationTime, const AnimNode* nodeData) const
{
	return glm::toMat4(_InterpolateQuat(animationTime, nodeData->mRotations));
}

glm::mat4 AnimatorSystem::_InterpolateScale(float animationTime, const AnimNode* nodeData) const
{
	return glm::scale(glm::mat4(1.0f), _InterpolateVec3(animationTime, nodeData->mScales));
}

glm::vec3 AnimatorSystem::_InterpolateVec3(float animationTime, const std::vector<KeyframeVec>& keyframes) const
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

glm::quat AnimatorSystem::_InterpolateQuat(float animationTime, const std::vector<KeyframeQuat>& keyframes) const
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

glm::mat4 AnimatorSystem::_InterpolateMatrices(const glm::mat4& matA, const glm::mat4& matB, float blendFactor) const
{
	// Extract position, scale, and rotation from both matrices

	// 1. Extract translations (position)
	auto posA = glm::vec3(matA[3]); // The position is in the 4th column (index 3) of the matrix
	auto posB = glm::vec3(matB[3]);

	// 2. Extract scales
	auto scaleA = glm::vec3(glm::length(matA[0]), glm::length(matA[1]), glm::length(matA[2]));
	auto scaleB = glm::vec3(glm::length(matB[0]), glm::length(matB[1]), glm::length(matB[2]));

	// 3. Extract rotations (as quaternions)
	glm::quat rotA = glm::quat_cast(matA);
	glm::quat rotB = glm::quat_cast(matB);

	// Interpolate (blend) each component
	glm::vec3 blendedPos = glm::mix(posA, posB, blendFactor);
	glm::vec3 blendedScale = glm::mix(scaleA, scaleB, blendFactor);
	glm::quat blendedRot = glm::slerp(rotA, rotB, blendFactor); // Spherical interpolation for rotation

	// Reconstruct the final blended matrix
	glm::mat4 blendedMatrix = glm::translate(glm::mat4(1.0f), blendedPos) * glm::mat4_cast(blendedRot);
	blendedMatrix = glm::scale(blendedMatrix, blendedScale);

	return blendedMatrix;
}

const AnimNode* AnimatorSystem::GetAnimNode(const Animation* animation, int nodeId)
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

void AnimatorSystem::_ApplyBlendTime(const AnimationComponent& animComp)
{
	for (const auto& transition : animComp.mTransitions)
	{
		if (transition.mTransitionFrom == animComp.mPrevAnimState && transition.mTransitionTo == animComp.
			mCurrentAnimState)
		{
			mCurrentAnimationTime = transition.mTargetedBlendTime;
			mPrevAnimationTime = transition.mTargetedBlendTime;
		}
	}
}

void AnimatorSystem::_SetJointMatrixCount(Entity* entity) const
{
	auto& mesh = entity->GetComponent<ModelComponent>();
	const auto& model = mResourceManager.GetModelFromId(mesh.mModelPath);

	//if (mesh.mHasBones) TODO: Figure out how to re-introduce
	//{
	mesh.mFinalTransforms.resize(model.GetNumJoints());
	//}
}
