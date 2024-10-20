#include "AnimationDescription.h"

#include "Util/FileSystem.h"

void AnimationDescription::ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry)
{
	entityRegistry.AddComponent<AnimationComponent>(entity, mInfos);
}

void AnimationDescription::ParseJSON(const nlohmann::json& json)
{
	std::string fullMapPath = ANIMATIONS_FOLDER;
	fullMapPath.append(json);

	const auto animJson = FileSystem::ReadJson(fullMapPath);

	for (const auto& item : animJson[LIST_STRING])
	{
		AnimInfo t = {item[FROM_STRING], item[TO_STRING], item[TRANSITION_TIME_STRING]};
		mInfos.push_back(t);
	}
}
