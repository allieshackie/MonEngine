#pragma once
#include "Entity/Descriptions/DescriptionBase.h"

class MeshDescription : public DescriptionBase
{
public:
	MeshDescription() = default;
	// Copy constructor/assignment operator
	MeshDescription(const MeshDescription& other) = default;
	MeshDescription& operator=(const MeshDescription& other) = default;
	// Move constructor/assignment operator
	MeshDescription(MeshDescription&& other) noexcept = default;
	MeshDescription& operator=(MeshDescription&& rhs) noexcept = default;

	~MeshDescription() override = default;

	void ApplyToEntity(Entity* entity, entt::registry& registry) override;

	static constexpr char JsonName[] = "mesh";
};
