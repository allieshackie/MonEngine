#pragma once
#include "DescriptionBase.h"

class TransformDescription : public DescriptionBase
{
public:
	TransformDescription() = default;
	// Copy constructor/assignment operator
	TransformDescription(const TransformDescription& other) = default;
	TransformDescription& operator=(const TransformDescription& other) = default;
	// Move constructor/assignment operator
	TransformDescription(TransformDescription&& other) noexcept = default;
	TransformDescription& operator=(TransformDescription&& rhs) noexcept = default;

	~TransformDescription() override = default;

	void ApplyToEntity(Entity* entity, entt::registry& registry) override;

	static constexpr char JsonName[] = "transform";
};
