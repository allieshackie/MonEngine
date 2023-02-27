#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "DescriptionBase.h"

/*
	JSON Parser can't handle comments, putting relevant information here:
	
	ground_tileset.json
	320px x 240px
	20 x 16 division
	tiles: 16 x 15px
*/

struct TileSetDescription : DescriptionBase
{
public:
	TileSetDescription() = default;

	void ApplyToEntity(EntityId entity, EntityRegistry& entityRegistry) override;

	const std::string& getTexturePath() const;
	int GetRows() const;
	int GetColumns() const;

	int GetTotalTiles() const;

	glm::vec4 GetClipForTile(int index) const;

	void SetRows(int rows);
	void setColumns(int columns);
	void UpdateSize(int rows, int columns);

	void ParseJSON(const nlohmann::json& json) override;

	static constexpr char JsonName[] = "tileset";
private:
	static constexpr char ID_STRING[] = "id";
	static constexpr char TEXTURE_STRING[] = "texture_path";
	static constexpr char SIZE_STRING[] = "rows_columns";

	nlohmann::json mJson;
	std::string mTexturePath;

	int mTilesetRows = 0;
	int mTilesetColumns = 0;
	glm::vec2 mTextureSize = {1.0f, 1.0f};
};
