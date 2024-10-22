#pragma once
#include "Graphics/Animation/Animation.h"

struct AnimTransition
{
	AnimationStates mTransitionFrom = AnimationStates::NONE;
	AnimationStates mTransitionTo = AnimationStates::NONE;

	float mTransitionTime = 0.0f;
	float mTargetedBlendTime = 0.0f;
};

struct AnimationComponent
{
	std::unordered_map<std::string, AnimationStates> mAnimationNames;
	std::vector<AnimTransition> mTransitions;

	// runtime
	bool mUpdated = false;
	AnimationStates mPrevAnimState = AnimationStates::NONE;
	AnimationStates mCurrentAnimState = AnimationStates::IDLE;
	float mBlendFactor = 1.0f;

	void TryTriggerAnimation(AnimationStates state)
	{
		if (mCurrentAnimState != state)
		{
			mPrevAnimState = mCurrentAnimState;
			mCurrentAnimState = state;
			mBlendFactor = 0.0f;
			mUpdated = true;
		}
	}
};
