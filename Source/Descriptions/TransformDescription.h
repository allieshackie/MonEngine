#pragma once

#include <glm/vec3.hpp>

#include "DescriptionBase.h"

struct TransformDescription : DescriptionBase
{
public:
	TransformDescription() = default;
	~TransformDescription() override = default;

	void ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry) override;
	void ParseJSON(const nlohmann::json& json) override;

	static constexpr char JsonName[] = "transform";

private:
	static constexpr char POSITION_STRING[] = "position";
	static constexpr char SIZE_STRING[] = "size";
	static constexpr char ROTATION_STRING[] = "rotation";

	glm::vec3 mPosition = {0, 0, 0};
	glm::vec3 mSize = {0, 0, 0};
	glm::vec3 mRotation = {0, 0, 0};
};
