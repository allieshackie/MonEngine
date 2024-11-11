#include "Entity/Components/MeshComponent.h"

#include "MeshDescription.h"

void MeshDescription::ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry)
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

	entityRegistry.AddComponent<MeshComponent>(entity, mesh);
}
