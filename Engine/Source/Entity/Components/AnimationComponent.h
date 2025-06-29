#pragma once
#include "cereal/types/unordered_map.hpp"
#include "Util/SerialUtil.h"

#include "Graphics/Animation/Animation.h"

struct AnimTransition
{
	int mTransitionFrom = 0;
	int mTransitionTo = 0;

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
	int mPrevAnimState = -1;
	int mCurrentAnimState = 5;
	float mBlendFactor = 1.0f;

	template <class Archive>
	void save(Archive& archive) const
	{
		archive(cereal::make_nvp("transitions", mTransitions));
	}

	template <class Archive>
	void load(Archive& archive)
	{
		cereal::make_optional_nvp(archive, "transitions", mTransitions);
	}

	void TryTriggerAnimation(int state)
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
