#pragma once
#include <glm/mat4x4.hpp>
#include "Util/SerialUtil.h"

struct MeshComponent
{
	// data driven
	std::string mMeshPath;
	bool mHasBones = false;

	// runtime
	int mMeshId = 0;
	std::vector<glm::mat4> mFinalTransforms;

	// debug runtime
	int mCurrentBoneIndex = 0;

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("mesh_path", mMeshPath),
		        cereal::make_nvp("has_bones", mHasBones));
	}
};
