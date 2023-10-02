#pragma once
#include <nlohmann/json.hpp>

/*
 * This description isn't associated with a component
 * Maps should be handled separately from entities
 * but still require data to be parsed and held
 */

struct MapAttributes
{
	MapAttributes(std::string id, int rows, int columns, std::array<uint32_t, 2> textureSize, std::string texturePath,
	              int textureRows, int textureColumns, std::string dataPath, std::vector<int> data)
		: mId(std::move(id)), mRows(rows), mColumns(columns), mTextureSize(textureSize),
		  mTexturePath(std::move(texturePath)), mTextureMapRows(textureRows), mTextureMapColumns(textureColumns),
		  mDataPath(std::move(dataPath)), mData(std::move(data))
	{
	}

	std::string mId;

	int mRows = 0;
	int mColumns = 0;

	std::array<uint32_t, 2> mTextureSize = {0, 0};

	std::string mTexturePath;
	int mTextureMapRows = 0;
	int mTextureMapColumns = 0;

	std::string mDataPath;
	std::vector<int> mData;
};

namespace MapHelpers
{
	std::unique_ptr<MapAttributes> CreateAttributes(const std::string& fileName);

	static constexpr char ID_STRING[] = "id";

	static constexpr char ROWS_STRING[] = "rows";
	static constexpr char COLUMNS_STRING[] = "columns";
	static constexpr char TEXTURE_SIZE_STRING[] = "texture_size";

	static constexpr char TEXTURE_DATA_STRING[] = "texture_data";
	static constexpr char TEXTURE_PATH_STRING[] = "texture_path";
	static constexpr char DATA_PATH_STRING[] = "data_path";
}
