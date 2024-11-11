#pragma once
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <map>

struct JointNode
{
	std::string mId;
	glm::mat4 mTransformation = glm::mat4(1.0f);
	// Convert bone from world space to bone space
	glm::mat4 mInverseBindMatrix = glm::mat4(1.0f);

	std::vector<int> mChildren;
};

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
	float mStartFrameTime = 0.0f;
	float mEndFrameTime = 0.0f;
	std::vector<AnimNode*> mAnimNodes;
};

enum AnimationStates
{
	NONE,
	IDLE,
	WALKING
};

inline std::map<std::string, AnimationStates> ANIM_NAMES = {
	{"Idle", AnimationStates::IDLE},
	{"Walk", AnimationStates::WALKING},
};

inline std::map<AnimationStates, std::string> ANIM_STRINGS = {
	{AnimationStates::IDLE, "Idle"},
	{AnimationStates::WALKING, "Walk"},
};

inline std::string ToAnimString(AnimationStates state)
{
	switch (state)
	{
	case AnimationStates::NONE: return "None";
	case AnimationStates::IDLE: return "Idle";
	case AnimationStates::WALKING: return "Walk";
	// add cases for other states...
	default: throw std::invalid_argument("Unknown AnimationState");
	}
}

inline AnimationStates FromAnimString(const std::string& str)
{
	if (str == "None") return AnimationStates::NONE;
	if (str == "Idle") return AnimationStates::IDLE;
	if (str == "Walk") return AnimationStates::WALKING;
	// add other string-to-enum mappings...
	throw std::invalid_argument("Unknown AnimationState: " + str);
}
