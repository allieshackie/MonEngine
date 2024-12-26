#include "Entity/Components/CollisionComponent.h"

#include "CollisionDescription.h"

void CollisionDescription::ApplyToEntity(Entity* entity, entt::registry& registry)
{
	std::istringstream jsonStream(mJson);

	CollisionComponent collider;
	try
	{
		cereal::JSONInputArchive archive(jsonStream);
		collider.serialize(archive);
	}
	catch (const cereal::Exception& e)
	{
		std::cerr << "CollisionComponent deserialization error: " << e.what() << std::endl;
		assert(false);
	}

	entity->AddComponent<CollisionComponent>(collider);
}
