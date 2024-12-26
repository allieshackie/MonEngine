#pragma once
#include "DescriptionBase.h"

#include "Entity/EntityRegistry.h"

class AnimationDescription : public DescriptionBase
{
public:
	AnimationDescription() = default;
	// Copy constructor/assignment operator
	AnimationDescription(const AnimationDescription& other) = default;
	AnimationDescription& operator=(const AnimationDescription& other) = default;
	// Move constructor/assignment operator
	AnimationDescription(AnimationDescription&& other) noexcept = default;
	AnimationDescription& operator=(AnimationDescription&& rhs) noexcept = default;

	~AnimationDescription() override = default;

	void ApplyToEntity(Entity* entity, entt::registry& registry) override = 0;

	static constexpr char JsonName[] = "animation";
};
