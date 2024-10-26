#include "Entity/Components/LightComponent.h"

#include "LightDescription.h"

void LightDescription::ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry)
{
	entityRegistry.AddComponent<LightComponent>(entity, mAmbient, mDiffuse, mSpecular, mIntensity,
	                                            static_cast<LightType>(mLightType));
}

void LightDescription::ParseJSON(const nlohmann::json& json)
{
	assert(json.contains(AMBIENT_STRING));
	auto& ambient = json[AMBIENT_STRING];
	assert(ambient.size() == 4);
	mAmbient = glm::vec4(ambient[0], ambient[1], ambient[2], ambient[3]);

	assert(json.contains(DIFFUSE_STRING));
	auto& diffuse = json[DIFFUSE_STRING];
	assert(diffuse.size() == 4);
	mDiffuse = glm::vec4(diffuse[0], diffuse[1], diffuse[2], diffuse[3]);

	assert(json.contains(SPECULAR_STRING));
	auto& specular = json[SPECULAR_STRING];
	assert(specular.size() == 4);
	mSpecular = glm::vec4(specular[0], specular[1], specular[2], specular[3]);

	if (json.contains(INTENSITY_STRING))
	{
		mIntensity = json[INTENSITY_STRING];
	}

	assert(json.contains(LIGHT_TYPE_STRING));
	mLightType = json[LIGHT_TYPE_STRING];
}
