#include "Entity/Components/TriggerVolumeComponent.h"

#include "TriggerVolumeDescription.h"

void TriggerVolumeDescription::ApplyToEntity(Entity* entity, entt::registry& registry)
{
	TriggerVolumeComponent& trigger = entity->AddComponent<TriggerVolumeComponent>();
	try
	{
		auto archive = FileSystem::CreateArchive(mJson);
		trigger.serialize(archive);
	}
	catch (const cereal::Exception& e)
	{
		std::cerr << "TriggerVolumeComponent deserialization error: " << e.what() << std::endl;
		assert(false);
	}
}
