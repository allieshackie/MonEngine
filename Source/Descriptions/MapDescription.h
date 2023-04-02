#pragma once
#include <nlohmann/json.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

/*
 * This description isn't associated with a component
 * Maps should be handled separately from entities
 * but still require data to be parsed and held
 */

struct MapDescription
{
public:
	MapDescription(const std::string& fileName);
	~MapDescription() = default;

	const std::string& GetId() { return mId; }

	int GetRows() const { return mRows; }
	int GetColumns() const { return mColumns; }
	int GetTileSize() const { return mTileSize; }

	glm::vec3 GetMapSize() const { return mMapSize; }

	glm::vec3 GetPosition() const { return mPosition; }
	glm::vec3 GetSize() const { return mSize; }
	float GetRotation() const { return mRotation; }

	const std::string& GetTexturePath() { return mTexturePath; }

	int GetTextureMapRows() const { return mTextureMapRows; }
	void SetTextureMapRows(int rows);

	int GetTextureMapColumns() const { return mTextureMapColumns; }
	void SetTextureMapColumns(int columns);

	std::vector<int>& GetMapData() { return mData; }

	bool GetRenderDebug() const { return mRenderDebug; }
	void SetRenderDebug(bool renderDebug);

	glm::vec4 GetClipForTile(int index) const;

	void CalculateTileDrawData(int tileIndex, glm::vec3& pos, glm::vec3& size, glm::vec4& clip);

	static constexpr char ID_STRING[] = "id";

	static constexpr char ROWS_STRING[] = "rows";
	static constexpr char COLUMNS_STRING[] = "columns";
	static constexpr char TILE_SIZE_STRING[] = "tile_size";

	static constexpr char TRANSFORM_STRING[] = "transform";
	static constexpr char POSITION_STRING[] = "position";
	static constexpr char SIZE_STRING[] = "size";
	static constexpr char ROTATION_STRING[] = "rotation";

	static constexpr char TEXTURE_DATA_STRING[] = "texture_data";
	static constexpr char TEXTURE_PATH_STRING[] = "texture_path";
	static constexpr char DATA_PATH_STRING[] = "data_path";

private:
	void _ParseJSON(const nlohmann::json& json);
	void _ReadFile(const char* fileName);

	std::string mId;

	int mRows = 0;
	int mColumns = 0;
	int mTileSize = 0;

	glm::vec3 mMapSize = {0, 0, 0};

	glm::vec3 mPosition = {0, 0, 0};
	glm::vec3 mSize = {0, 0, 0};
	float mRotation = 0;

	std::string mTexturePath;
	int mTextureMapRows = 0;
	int mTextureMapColumns = 0;

	std::vector<int> mData;

	bool mRenderDebug = false;
};
