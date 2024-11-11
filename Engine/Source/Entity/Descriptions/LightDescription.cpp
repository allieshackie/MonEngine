#include "Entity/Components/LightComponent.h"

#include "LightDescription.h"

void LightDescription::ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry)
{
	std::istringstream jsonStream(mJson);
	LightComponent light;

	try
	{
		cereal::JSONInputArchive archive(jsonStream);
		light.serialize(archive);
	}
	catch (const cereal::Exception& e)
	{
		std::cerr << "LightComponent deserialization error: " << e.what() << std::endl;
		assert(false);
	}

	entityRegistry.AddComponent<LightComponent>(entity, light);
}
