#pragma once

#include "DescriptionBase.h"

class SpriteDescription : public DescriptionBase
{
public:
	SpriteDescription() = default;
	// Copy constructor/assignment operator
	SpriteDescription(const SpriteDescription& other) = default;
	SpriteDescription& operator=(const SpriteDescription& other) = default;
	// Move constructor/assignment operator
	SpriteDescription(SpriteDescription&& other) noexcept = default;
	SpriteDescription& operator=(SpriteDescription&& rhs) noexcept = default;

	~SpriteDescription() override = default;

	void ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry) override;
	void ParseJSON(const nlohmann::json& json) override;

	static constexpr char JsonName[] = "sprite";

private:
	static constexpr char RESOURCE_PATH_STRING[] = "resource_path";
	static constexpr char ROWS_STRING[] = "rows";
	static constexpr char COLUMNS_STRING[] = "columns";

	std::string mResourcePath;

	// optional
	int mRows = 0;
	int mColumns = 0;
};
