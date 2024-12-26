#pragma once
#include <glm/vec3.hpp>
#include <entt/entt.hpp>
#include "Util/SerialUtil.h"

class Camera;

struct MapData
{
	std::string mName;
	glm::vec3 mPosition = {0, 0, 0};
	glm::vec3 mSize = {0, 0, 0};
	glm::vec3 mRotation = {0, 0, 0};
	glm::vec4 mColor = {0, 0, 0, 0};
	float mTileSize = 0.0f;
	bool mHasDimension = false;

	template <class Archive>
	void save(Archive& ar) const
	{
		ar(cereal::make_nvp("name", mName),
		   cereal::make_nvp("position", mPosition),
		   cereal::make_nvp("size", mSize),
		   cereal::make_nvp("rotation", mRotation),
		   cereal::make_nvp("color", mColor),
		   cereal::make_nvp("tile_size", mTileSize),
		   cereal::make_nvp("has_dimension", mHasDimension));
	}

	template <class Archive>
	void load(Archive& ar)
	{
		ar(cereal::make_nvp("name", mName),
		   cereal::make_nvp("position", mPosition),
		   cereal::make_nvp("size", mSize),
		   cereal::make_nvp("rotation", mRotation),
		   cereal::make_nvp("has_dimension", mHasDimension)
		);

		cereal::make_optional_nvp(ar, "color", mColor);
		cereal::make_optional_nvp(ar, "tile_size", mTileSize);
	}
};

struct CameraData
{
	glm::vec3 mCameraPos = {0.0f, 0.0f, 0.0f};
	glm::vec3 mCameraFront = {0.0f, 0.0f, 0.0f};
	glm::vec3 mCameraUp = {0.0f, 0.0f, 0.0f};

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("position", mCameraPos),
		        cereal::make_nvp("front", mCameraFront),
		        cereal::make_nvp("up", mCameraUp));
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

struct MonScene
{
	MapData mMapData;
	CameraData mCameraData;
	std::vector<EntityData> mEntityDefinitions;
	std::vector<std::string> mScripts;

	std::unique_ptr<Camera> mCamera = nullptr;
	entt::registry mRegistry;

	template <class Archive>
	void save(Archive& ar) const
	{
		ar(cereal::make_nvp("maps", mMapData),
		   cereal::make_nvp("camera", mCameraData),
		   cereal::make_nvp("entities", mEntityDefinitions),
		   cereal::make_nvp("scripts", mScripts)
		);
	}

	template <class Archive>
	void load(Archive& ar)
	{
		ar(cereal::make_nvp("camera", mCameraData));

		cereal::make_optional_nvp(ar, "maps", mMapData);
		cereal::make_optional_nvp(ar, "entities", mEntityDefinitions);
		cereal::make_optional_nvp(ar, "scripts", mScripts);
	}
};
