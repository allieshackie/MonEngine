#include "Entity/Components/LightComponent.h"

#include "LightDescription.h"

void LightDescription::ApplyToEntity(Entity* entity, entt::registry& registry)
{
	LightComponent light;
	try
	{
		auto archive = FileSystem::CreateArchive(mJson);
		light.serialize(archive);
	}
	catch (const cereal::Exception& e)
	{
		std::cerr << "LightComponent deserialization error: " << e.what() << std::endl;
		assert(false);
	}

	entity->AddComponent<LightComponent>(light);
}
