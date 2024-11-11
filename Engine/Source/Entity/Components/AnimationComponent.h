#pragma once
#include "Util/SerialUtil.h"
#include "cereal/types/unordered_map.hpp"

#include "Graphics/Animation/Animation.h"

struct AnimTransition
{
	AnimationStates mTransitionFrom = AnimationStates::NONE;
	AnimationStates mTransitionTo = AnimationStates::NONE;

	float mTransitionTime = 0.0f;
	float mTargetedBlendTime = 0.0f;

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("from", mTransitionFrom),
		        cereal::make_nvp("to", mTransitionTo),
		        cereal::make_nvp("transition_time", mTransitionTime),
		        cereal::make_nvp("targeted_blend_time", mTargetedBlendTime));
	}
};

struct AnimationComponent
{
	std::unordered_map<std::string, int> mAnimationNames;
	std::vector<AnimTransition> mTransitions;

	// runtime
	bool mUpdated = false;
	AnimationStates mPrevAnimState = AnimationStates::NONE;
	AnimationStates mCurrentAnimState = AnimationStates::IDLE;
	float mBlendFactor = 1.0f;

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("animations", mAnimationNames),
		        cereal::make_nvp("transitions", mTransitions));
	}

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
