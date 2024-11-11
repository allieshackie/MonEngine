#include "Entity/Components/TransformComponent.h"

#include "TransformDescription.h"

void TransformDescription::ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry)
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

	entityRegistry.AddComponent<TransformComponent>(entity, transform);
}
