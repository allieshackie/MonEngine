#include "Entity/Components/MeshComponent.h"

#include "MeshDescription.h"

void MeshDescription::ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry)
{
	entityRegistry.AddComponent<MeshComponent>(entity, mMeshPath);
}

void MeshDescription::ParseJSON(const nlohmann::json& json)
{
	assert(json.contains(MESH_PATH_STRING));
	mMeshPath = json[MESH_PATH_STRING];
}
