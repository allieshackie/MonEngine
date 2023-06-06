#pragma once
#include <nlohmann/json.hpp>

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

	std::array<uint32_t, 2> GetTextureSize() const { return mTextureSize; }

	const std::string& GetTexturePath() { return mTexturePath; }

	int GetTextureMapRows() const { return mTextureMapRows; }
	void SetTextureMapRows(int rows);

	int GetTextureMapColumns() const { return mTextureMapColumns; }
	void SetTextureMapColumns(int columns);

	std::vector<int>& GetMapData() { return mData; }
	const std::string& GetDataPath() const { return mDataPath; }

	static constexpr char ID_STRING[] = "id";

	static constexpr char ROWS_STRING[] = "rows";
	static constexpr char COLUMNS_STRING[] = "columns";
	static constexpr char TEXTURE_SIZE_STRING[] = "texture_size";

	static constexpr char TEXTURE_DATA_STRING[] = "texture_data";
	static constexpr char TEXTURE_PATH_STRING[] = "texture_path";
	static constexpr char DATA_PATH_STRING[] = "data_path";

private:
	void _ParseJSON(const nlohmann::json& json);
	void _ReadFile(const char* fileName);

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
