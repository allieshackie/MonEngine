#pragma once
#include <glm/vec3.hpp>

#include "Entity/EntityRegistry.h"
#include "Entity/Descriptions/DescriptionBase.h"

class CollisionDescription : public DescriptionBase
{
public:
	CollisionDescription() = default;
	// Copy constructor/assignment operator
	CollisionDescription(const CollisionDescription& other) = default;
	CollisionDescription& operator=(const CollisionDescription& other) = default;
	// Move constructor/assignment operator
	CollisionDescription(CollisionDescription&& other) noexcept = default;
	CollisionDescription& operator=(CollisionDescription&& rhs) noexcept = default;

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
