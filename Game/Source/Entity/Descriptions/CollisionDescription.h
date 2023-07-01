#pragma once
#include <glm/vec3.hpp>

#include "Entity/EntityRegistry.h"
#include "Entity/Descriptions/DescriptionBase.h"

struct CollisionDescription : DescriptionBase
{
public:
	CollisionDescription() = default;
	~CollisionDescription() override = default;

	void ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry) override;

	void ParseJSON(const nlohmann::json& json) override;

	static constexpr char JsonName[] = "collider";

private:
	static constexpr char COLLIDER_SHAPE_STRING[] = "collider_shape";
	static constexpr char SIZE_STRING[] = "size";

	int mColliderShape = 0;
	glm::vec3 mSize = {0, 0, 0};
};
