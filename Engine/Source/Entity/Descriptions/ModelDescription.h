#pragma once
#include "Entity/Descriptions/DescriptionBase.h"

class ModelDescription : public DescriptionBase
{
public:
	ModelDescription() = default;
	// Copy constructor/assignment operator
	ModelDescription(const ModelDescription& other) = default;
	ModelDescription& operator=(const ModelDescription& other) = default;
	// Move constructor/assignment operator
	ModelDescription(ModelDescription&& other) noexcept = default;
	ModelDescription& operator=(ModelDescription&& rhs) noexcept = default;

	~ModelDescription() override = default;

	void ApplyToEntity(Entity* entity, entt::registry& registry) override;

	static constexpr char JsonName[] = "model";
};
