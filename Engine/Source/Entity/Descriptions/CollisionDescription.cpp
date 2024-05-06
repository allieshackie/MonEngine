#include "CollisionDescription.h"

void CollisionDescription::ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry)
{
	entityRegistry.AddComponent<CollisionComponent>(entity, mColliderShape, mSize, -1, false);
}

void CollisionDescription::ParseJSON(const nlohmann::json& json)
{
	assert(json.contains(COLLIDER_SHAPE_STRING));
	mColliderShape = json[COLLIDER_SHAPE_STRING];

	assert(json.contains(SIZE_STRING));
	auto& size = json[SIZE_STRING];
	assert(size.size() == 3);
	mSize = glm::vec3(size[0], size[1], size[2]);
}
