#include "Entity/Components/TransformComponent.h"

#include "TransformDescription.h"

void TransformDescription::ApplyToEntity(Entity* entity, entt::registry& registry)
{
	std::istringstream jsonStream(mJson);
	TransformComponent transform;

	try
	{
		cereal::JSONInputArchive archive(jsonStream);
		transform.serialize(archive);
	}
	catch (const cereal::Exception& e)
	{
		std::cerr << "TransformComponent deserialization error: " << e.what() << std::endl;
		assert(false);
	}

	entity->AddComponent<TransformComponent>(transform);
}
