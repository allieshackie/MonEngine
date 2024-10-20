#pragma once

struct AnimInfo
{
	std::string mTransitionFrom;
	std::string mTransitionTo;
	float mTransitionTime = 0.0f;
};

struct AnimationComponent
{
	std::vector<AnimInfo> mInfos;
};
