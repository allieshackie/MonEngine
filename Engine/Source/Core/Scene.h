#pragma once
#include <glm/vec3.hpp>
#include "Util/SerialUtil.h"
#include "Entity/PrefabRegistry.h"

struct CameraData
{
	glm::vec3 mCameraPos = {0.0f, 0.0f, 0.0f};
	glm::vec3 mCameraFront = {0.0f, 0.0f, 0.0f};
	glm::vec3 mCameraUp = {0.0f, 0.0f, 0.0f};
	glm::vec3 mCameraFollowOffset = { 0.0f, 10.0f, -20.0f };
	glm::vec3 mCameraLookOffset = { 0.0f, 6.0f, -1.0f };
	bool mFollowCam = true;

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("position", mCameraPos),
		        cereal::make_nvp("front", mCameraFront),
		        cereal::make_nvp("up", mCameraUp),
		        cereal::make_nvp("follow_cam", mFollowCam));

		cereal::make_optional_nvp(archive, "camera_follow_offset", mCameraFollowOffset);
		cereal::make_optional_nvp(archive, "camera_look_offset", mCameraLookOffset);
	}
};

struct EntityData
{
	std::string mPrefab;
	std::vector<SerializedComponent> mOverrides;
};

class MonScene
{
public:
	const CameraData& GetCameraData() const { return mCameraData; }
	const std::vector<EntityData>& GetEntityOverrides() const { return mEntityOverrides; }
	const std::vector<std::string>& GetScripts() const { return mScripts; }
	void AddEntityOverride(EntityData& data) { mEntityOverrides.push_back(data); }

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("camera", mCameraData));

		cereal::make_optional_nvp(archive, "scripts", mScripts);
	}

private:
	CameraData mCameraData;
	std::vector<EntityData> mEntityOverrides;
	std::vector<std::string> mScripts;
};
