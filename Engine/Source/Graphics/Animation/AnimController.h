#pragma once
#include "Animation.h"

struct AnimTransition
{
	AnimationStates mTransitionFrom = AnimationStates::NONE;
	AnimationStates mTransitionTo = AnimationStates::NONE;

	float mTransitionTime = 0.0f;
};

class AnimController
{
public:
	void AddTransition(AnimTransition* newTransition);
	AnimTransition* TryGetTransition(AnimationStates stateFrom) const;

private:
	std::vector<AnimTransition*> mTransitions;
	std::unordered_map<AnimTransition, std::string> mAnimationNames;
};
