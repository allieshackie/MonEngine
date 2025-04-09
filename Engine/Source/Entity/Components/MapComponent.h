#pragma once
#include "Util/SerialUtil.h"

struct MapComponent
{
	std::string mId;

	std::string mMeshPath;

	std::string mTexturePath;
	glm::vec2 mTextureSize = {0, 0};
	glm::vec2 mTextureTiling = {0, 0};

	std::string mDataPath;
	std::vector<int> mData;

	glm::vec3 mColor = {-1, -1, -1};

	// Runtime
	int mGeneratedTextureId = -1;

	template <class Archive>
	void save(Archive& archive) const
	{
		archive(cereal::make_nvp("id", mId),
		        cereal::make_nvp("mesh_path", mMeshPath),
		        cereal::make_nvp("texture_path", mTexturePath),
		        cereal::make_nvp("texture_size", mTextureSize),
		        cereal::make_nvp("texture_tiling", mTextureTiling),
		        cereal::make_nvp("data_path", mDataPath));
	}

	template <class Archive>
	void load(Archive& archive)
	{
		archive(cereal::make_nvp("id", mId), cereal::make_nvp("mesh_path", mMeshPath));

		cereal::make_optional_nvp(archive, "texture_path", mTexturePath);
		cereal::make_optional_nvp(archive, "texture_size", mTextureSize);
		cereal::make_optional_nvp(archive, "texture_tiling", mTextureTiling);
		cereal::make_optional_nvp(archive, "data_path", mDataPath);

		cereal::make_optional_nvp(archive, "color", mColor);
	}
};
