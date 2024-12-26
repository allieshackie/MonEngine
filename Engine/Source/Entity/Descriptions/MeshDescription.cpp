#include "Entity/Components/MeshComponent.h"

#include "MeshDescription.h"

void MeshDescription::ApplyToEntity(Entity* entity, entt::registry& registry)
{
	std::istringstream jsonStream(mJson);
	MeshComponent mesh;

	try
	{
		cereal::JSONInputArchive archive(jsonStream);
		mesh.serialize(archive);
	}
	catch (const cereal::Exception& e)
	{
		std::cerr << "MeshComponent deserialization error: " << e.what() << std::endl;
		assert(false);
	}

	entity->AddComponent<MeshComponent>(mesh);
}
