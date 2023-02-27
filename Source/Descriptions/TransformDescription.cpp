#include "TransformComponent.h"

#include "TransformDescription.h"

void TransformDescription::ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry)
{
	entityRegistry.AddComponent<TransformComponent>(entity, mPosition, mSize, mRotation);
}

void TransformDescription::ParseJSON(const nlohmann::json& json)
{
	assert(json.contains(POSITION_STRING));
	auto position = json[POSITION_STRING];
	assert(position.size() == 3);
	mPosition = glm::vec3(position[0], position[1], position[2]);

	assert(json.contains(SIZE_STRING));
	auto size = json[SIZE_STRING];
	assert(size.size() == 3);
	mSize = glm::vec3(size[0], size[1], size[2]);

	assert(json.contains(ROTATION_STRING));
	mRotation = json[ROTATION_STRING];
}
