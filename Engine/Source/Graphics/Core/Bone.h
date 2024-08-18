#pragma once
#include <vector>
#include <glm/vec3.hpp>
#include <glm/ext/quaternion_float.hpp>
#include "assimp/anim.h"

struct KeyframeVec
{
	glm::vec3 data = {0, 0, 0};
	float timeStamp = 0.0f;
};

struct KeyframeQuat
{
	glm::quat data;
	float timeStamp = 0.0f;
};

class Bone
{
public:
	Bone(const std::string& name, const aiNodeAnim* channel);
private:
	std::vector<KeyframeVec> mPositions;
	std::vector<KeyframeQuat> mRotations;
	std::vector<KeyframeVec> mScales;

	int mNumPositions = 0;
	int mNumRotations = 0;
	int mNumScales = 0;

	glm::mat4 mLocalTransform;
	std::string mName;
	int mId = 0;
};
