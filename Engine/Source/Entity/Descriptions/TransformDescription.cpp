#include "Entity/Components/TransformComponent.h"

#include "TransformDescription.h"

void TransformDescription::ApplyToEntity(Entity* entity, entt::registry& registry)
{
	TransformComponent transform;
	try
	{
		auto archive = FileSystem::CreateArchive(mJson);
		transform.serialize(archive);
	}
	catch (const cereal::Exception& e)
	{
		std::cerr << "TransformComponent deserialization error: " << e.what() << std::endl;
		assert(false);
	}

	entity->AddComponent<TransformComponent>(transform);
}
