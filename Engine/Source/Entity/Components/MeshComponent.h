#pragma once
#include <glm/ext/matrix_float4x4.hpp>

struct MeshComponent
{
	// data driven
	std::string mMeshPath;
	bool mHasBones = false;

	// runtime
	int mMeshId = 0;
	std::string mCurrentAnimation = "Simple";
	std::vector<glm::mat4> mFinalTransforms;

	// debug runtime
	int mCurrentBoneIndex = 0;
};
