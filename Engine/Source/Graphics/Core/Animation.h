#pragma once
#include <glm/vec3.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <vector>

struct aiNode;

struct BoneNode
{
	std::string mId;
	glm::mat4 mTransformation = glm::mat4(1.0f);
	std::vector<BoneNode*> mChildren;
};

struct KeyframeVec
{
	glm::vec3 mData = {0, 0, 0};
	double mTimeStamp = 0.0;
};

struct KeyframeQuat
{
	glm::quat mData;
	double mTimeStamp = 0.0;
};

struct AnimNode
{
	std::string mName;

	std::vector<KeyframeVec> mPositions;
	std::vector<KeyframeQuat> mRotations;
	std::vector<KeyframeVec> mScales;

	int mNumPositions = 0;
	int mNumRotations = 0;
	int mNumScales = 0;
};

struct Animation
{
	double mDuration = 0.0;
	double mTicksPerSecond = 0;
	std::vector<AnimNode*> mBoneAnimNodes;
};
