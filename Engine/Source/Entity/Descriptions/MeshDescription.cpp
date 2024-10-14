#include "Entity/Components/MeshComponent.h"

#include "MeshDescription.h"

void MeshDescription::ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry)
{
	entityRegistry.AddComponent<MeshComponent>(entity, mMeshPath, mHasBones);
}

void MeshDescription::ParseJSON(const nlohmann::json& json)
{
	assert(json.contains(MESH_PATH_STRING));
	mMeshPath = json[MESH_PATH_STRING];

	if (json.contains(HAS_BONES_STRING))
	{
		mHasBones = json[HAS_BONES_STRING];
	}
}
