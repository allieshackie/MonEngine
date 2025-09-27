#pragma once
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

struct KeyframeVec
{
	glm::vec3 mData = {0, 0, 0};
	float mTimeStamp = 0.0f;
};

struct KeyframeQuat
{
	glm::quat mData;
	float mTimeStamp = 0.0f;
};

struct AnimNode
{
	int mNodeIndex = -1;

	std::vector<KeyframeVec> mPositions;
	std::vector<KeyframeQuat> mRotations;
	std::vector<KeyframeVec> mScales;
};

struct Animation
{
	std::string mName;
	float mStartFrameTime = 0.0f;
	float mEndFrameTime = 0.0f;
	std::vector<AnimNode*> mAnimNodes;
};
