#pragma once
#include <glm/mat4x4.hpp>
#include "Util/SerialUtil.h"

struct ModelComponent
{
	// data driven
	std::string mModelPath;

	// runtime
	int mModelId = 0;
	std::vector<glm::mat4> mFinalTransforms;

	// debug runtime
	int mCurrentBoneIndex = 0;

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("model_path", mModelPath));
	}
};
