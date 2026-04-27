#pragma once
#include <glm/vec3.hpp>
#include "Util/SerialUtil.h"

struct TerrainData
{
	std::string mName;
	glm::vec3 mPosition = {0, 0, 0};
	glm::vec3 mSize = {0, 0, 0};
	glm::vec3 mRotation = {0, 0, 0};

	template <class Archive>
	void save(Archive& ar) const
	{
		ar(cereal::make_nvp("name", mName),
		   cereal::make_nvp("position", mPosition),
		   cereal::make_nvp("size", mSize),
		   cereal::make_nvp("rotation", mRotation));
	}

	template <class Archive>
	void load(Archive& ar)
	{
		ar(cereal::make_nvp("name", mName),
		   cereal::make_nvp("position", mPosition),
		   cereal::make_nvp("size", mSize),
		   cereal::make_nvp("rotation", mRotation));
	}
};

struct CameraData
{
	glm::vec3 mCameraPos = {0.0f, 0.0f, 0.0f};
	glm::vec3 mCameraFront = {0.0f, 0.0f, 0.0f};
	glm::vec3 mCameraUp = {0.0f, 0.0f, 0.0f};
	bool mFollowCam = true;

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("position", mCameraPos),
		        cereal::make_nvp("front", mCameraFront),
		        cereal::make_nvp("up", mCameraUp),
		        cereal::make_nvp("follow_cam", mFollowCam));
	}
};

struct EntityData
{
	std::string mName;
	glm::vec3 mPosition = {0.0f, 0.0f, 0.0f};
	glm::vec3 mColor = {0.0f, 0.0f, 0.0f};

	template <class Archive>
	void save(Archive& ar) const
	{
		ar(cereal::make_nvp("name", mName),
		   cereal::make_nvp("position", mPosition),
		   cereal::make_nvp("color", mColor));
	}

	template <class Archive>
	void load(Archive& ar)
	{
		ar(cereal::make_nvp("name", mName),
		   cereal::make_nvp("position", mPosition));

		cereal::make_optional_nvp(ar, "color", mColor);
	}
};

class MonScene
{
public:
	const CameraData& GetCameraData() const { return mCameraData; }
	const std::vector<EntityData>& GetEntityDefinitions() const { return mEntityDefinitions; }
	const std::vector<std::string>& GetScripts() const { return mScripts; }

	template <class Archive>
	void save(Archive& ar) const
	{
		ar(cereal::make_nvp("camera", mCameraData),
		   cereal::make_nvp("entities", mEntityDefinitions),
		   cereal::make_nvp("scripts", mScripts),
		);
	}

	template <class Archive>
	void load(Archive& ar)
	{
		ar(cereal::make_nvp("camera", mCameraData));

		cereal::make_optional_nvp(ar, "entities", mEntityDefinitions);
		cereal::make_optional_nvp(ar, "scripts", mScripts);
	}

private:
	CameraData mCameraData;
	std::vector<EntityData> mEntityDefinitions;
	std::vector<std::string> mScripts;
};
