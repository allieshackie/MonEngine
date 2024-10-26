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
	static constexpr char AMBIENT_STRING[] = "ambient";
	static constexpr char DIFFUSE_STRING[] = "diffuse";
	static constexpr char SPECULAR_STRING[] = "specular";
	static constexpr char INTENSITY_STRING[] = "intensity";
	static constexpr char LIGHT_TYPE_STRING[] = "light_type";

	glm::vec4 mAmbient = {0, 0, 0, 0};
	glm::vec4 mDiffuse = {0, 0, 0, 0};
	glm::vec4 mSpecular = {0, 0, 0, 0};
	float mIntensity = 1.0f;
	int mLightType = 0;
};
