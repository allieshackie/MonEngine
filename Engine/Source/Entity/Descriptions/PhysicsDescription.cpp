#include "Entity/Components/PhysicsComponent.h"

#include "PhysicsDescription.h"

void PhysicsDescription::ApplyToEntity(Entity* entity, entt::registry& registry)
{
	std::istringstream jsonStream(mJson);
	PhysicsComponent physics;

	try
	{
		cereal::JSONInputArchive archive(jsonStream);
		physics.serialize(archive);
	}
	catch (const cereal::Exception& e)
	{
		std::cerr << "PhysicsComponent deserialization error: " << e.what() << std::endl;
		assert(false);
	}

	entity->AddComponent<PhysicsComponent>(physics);
}
