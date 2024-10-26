#include "Entity/Components/LightComponent.h"

#include "LightDescription.h"

void LightDescription::ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry)
{
	entityRegistry.AddComponent<LightComponent>(entity, mColor, static_cast<LightType>(mLightType));
}

void LightDescription::ParseJSON(const nlohmann::json& json)
{
	assert(json.contains(COLOR_STRING));
	auto& color = json[COLOR_STRING];
	assert(color.size() == 3);
	mColor = glm::vec3(color[0], color[1], color[2]);

	assert(json.contains(LIGHT_TYPE_STRING));
	mLightType = json[LIGHT_TYPE_STRING];
}
