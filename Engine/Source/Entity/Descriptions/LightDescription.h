#pragma once
#include <glm/vec3.hpp>

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

	void ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry) override;
	void ParseJSON(const nlohmann::json& json) override;

	static constexpr char JsonName[] = "light";

private:
	static constexpr char COLOR_STRING[] = "color";
	static constexpr char LIGHT_TYPE_STRING[] = "light_type";

	glm::vec3 mColor = {0, 0, 0};
	int mLightType = 0;
};
