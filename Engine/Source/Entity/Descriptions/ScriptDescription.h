#pragma once
#include "DescriptionBase.h"

class ScriptDescription : public DescriptionBase
{
public:
	ScriptDescription() = default;
	// Copy constructor/assignment operator
	ScriptDescription(const ScriptDescription& other) = default;
	ScriptDescription& operator=(const ScriptDescription& other) = default;
	// Move constructor/assignment operator
	ScriptDescription(ScriptDescription&& other) noexcept = default;
	ScriptDescription& operator=(ScriptDescription&& rhs) noexcept = default;

	~ScriptDescription() override = default;

	void ApplyToEntity(Entity* entity, entt::registry& registry) override;

	static constexpr char JsonName[] = "script";
};
