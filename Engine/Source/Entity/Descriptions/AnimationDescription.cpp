#include "AnimationDescription.h"

#include "Util/FileSystem.h"

void AnimationDescription::ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry)
{
	entityRegistry.AddComponent<AnimationComponent>(entity, mAnimations, mTransitions);
}

void AnimationDescription::ParseJSON(const nlohmann::json& json)
{
	std::string fullMapPath = ANIMATIONS_FOLDER;
	fullMapPath.append(json);

	const auto animJson = FileSystem::ReadJson(fullMapPath);

	assert(animJson.contains(ANIMATIONS_STRING));
	for (const auto& anim : animJson[ANIMATIONS_STRING])
	{
		// Each item in the array is a single key-value pair
		for (const auto& [key, value] : anim.items())
		{
			mAnimations[key] = value;
		}
	}

	if (animJson.contains(LIST_STRING))
	{
		for (const auto& item : animJson[LIST_STRING])
		{
			AnimTransition t = {
				mAnimations[item[FROM_STRING]], mAnimations[item[TO_STRING]], item[TRANSITION_TIME_STRING],
				item[TARGETED_BLEND_TIME_STRING]
			};
			mTransitions.push_back(t);
		}
	}
}
