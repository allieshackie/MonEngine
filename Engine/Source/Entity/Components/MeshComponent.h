#pragma once

struct MeshComponent
{
	// data driven
	std::string mMeshPath;
	bool mHasBones = false;

	// runtime
	int mMeshId = 0;
	std::string mCurrentAnimation = "mixamo.com";

	// debug runtime
	int mCurrentBoneIndex = 0;
};
