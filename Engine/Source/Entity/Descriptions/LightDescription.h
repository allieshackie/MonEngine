#pragma once
#include "DescriptionBase.h"

class LightDescription : public DescriptionBase
{
public:
	LightDescription() = default;
	// Copy constructor/assignment operator
	LightDescription(const LightDescription& other) = default;
	LightDescription& operator=(const LightDescription& other) = default;
	// Move constructor/assignment operator
	LightDescription(LightDescription&& other) noexcept = default;
	LightDescription& operator=(LightDescription&& rhs) noexcept = default;

	~LightDescription() override = default;

	void ApplyToEntity(Entity* entity, entt::registry& registry) override = 0;

	static constexpr char JsonName[] = "light";
};
