#include "Entity/Components/MeshComponent.h"

#include "MeshDescription.h"

void MeshDescription::ApplyToEntity(Entity* entity, entt::registry& registry)
{
	MeshComponent mesh;
	try
	{
		auto archive = FileSystem::CreateArchive(mJson);
		mesh.serialize(archive);
	}
	catch (const cereal::Exception& e)
	{
		std::cerr << "MeshComponent deserialization error: " << e.what() << std::endl;
		assert(false);
	}

	entity->AddComponent<MeshComponent>(mesh);
}
