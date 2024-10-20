#include "AnimController.h"

void AnimController::AddTransition(AnimTransition* newTransition)
{
	mTransitions.push_back(newTransition);
}

AnimTransition* AnimController::TryGetTransition(AnimationStates stateFrom) const
{
	for (const auto transition : mTransitions)
	{
		if (transition->mTransitionFrom == stateFrom)
		{
			return transition;
		}
	}
	return nullptr;
}
