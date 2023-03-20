#pragma once

#include "DescriptionBase.h"

struct MapDescription : DescriptionBase
{
public:
	MapDescription() = default;
	~MapDescription() override = default;

	void ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry) override;

	void ParseJSON(const nlohmann::json& json) override;

	static constexpr char JsonName[] = "map";

private:
	static constexpr char ROWS_STRING[] = "rows";
	static constexpr char COLUMNS_STRING[] = "columns";
	static constexpr char TILE_SIZE_STRING[] = "tile_size";
	static constexpr char DATA_STRING[] = "data";

	int mRows = 0;
	int mColumns = 0;
	int mTileSize = 0;
	std::vector<int> mData;
};
