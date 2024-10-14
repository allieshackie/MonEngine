#pragma once
#include <glm/mat4x4.hpp>

struct MeshComponent
{
	// data driven
	std::string mMeshPath;
	bool mHasBones = false;

	// runtime
	int mMeshId = 0;
	std::string mCurrentAnimation = "Walk";
	std::vector<glm::mat4> mFinalTransforms;

	// debug runtime
	int mCurrentBoneIndex = 0;
};
