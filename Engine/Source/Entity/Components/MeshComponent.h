#pragma once
#include <glm/ext/matrix_float4x4.hpp>

struct MeshComponent
{
	// data driven
	std::string mMeshPath;

	// runtime
	int mMeshId = 0;
	std::string mCurrentAnimation;
	std::vector<glm::mat4> mFinalTransforms;
};
