#pragma once
#include "DescriptionBase.h"

class TriggerVolumeDescription : public DescriptionBase
{
public:
	TriggerVolumeDescription() = default;
	// Copy constructor/assignment operator
	TriggerVolumeDescription(const TriggerVolumeDescription& other) = default;
	TriggerVolumeDescription& operator=(const TriggerVolumeDescription& other) = default;
	// Move constructor/assignment operator
	TriggerVolumeDescription(TriggerVolumeDescription&& other) noexcept = default;
	TriggerVolumeDescription& operator=(TriggerVolumeDescription&& rhs) noexcept = default;

	~TriggerVolumeDescription() override = default;

	void ApplyToEntity(Entity* entity, entt::registry& registry) override;

	static constexpr char JsonName[] = "triggerVolume";
};
