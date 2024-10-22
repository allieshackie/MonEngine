#pragma once
#include "DescriptionBase.h"

#include "Entity/EntityRegistry.h"
#include "Entity/Components/AnimationComponent.h"

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

	void ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry) override;
	void ParseJSON(const nlohmann::json& json) override;

	static constexpr char JsonName[] = "animation";

private:
	static constexpr char ANIMATIONS_STRING[] = "animations";

	static constexpr char LIST_STRING[] = "transitions";
	static constexpr char TO_STRING[] = "to";
	static constexpr char FROM_STRING[] = "from";
	static constexpr char TRANSITION_TIME_STRING[] = "transition_time";
	static constexpr char TARGETED_BLEND_TIME_STRING[] = "targeted_blend_time";

	std::unordered_map<std::string, AnimationStates> mAnimations;
	std::vector<AnimTransition> mTransitions;
};
