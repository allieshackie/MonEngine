#include "Entity/Components/PhysicsComponent.h"

#include "PhysicsDescription.h"

void PhysicsDescription::ApplyToEntity(Entity* entity, entt::registry& registry)
{
	PhysicsComponent& physics = entity->AddComponent<PhysicsComponent>();
	try
	{
		auto archive = FileSystem::CreateArchive(mJson);
		physics.serialize(archive);
	}
	catch (const cereal::Exception& e)
	{
		std::cerr << "PhysicsComponent deserialization error: " << e.what() << std::endl;
		assert(false);
	}
}
