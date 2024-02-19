#include "Core/FileSystem.h"

#include "MapAttributes.h"

std::unique_ptr<MapAttributes> MapHelpers::CreateAttributes(const std::string& fileName)
{
	const auto json = FileSystem::ReadJson(fileName);
	assert(json.contains(ID_STRING));
	std::string id = json[ID_STRING];

	assert(json.contains(ROWS_STRING));
	int rows = json[ROWS_STRING];
	assert(json.contains(COLUMNS_STRING));
	int columns = json[COLUMNS_STRING];

	assert(json.contains(TEXTURE_DATA_STRING));
	const auto textureData = json[TEXTURE_DATA_STRING];
	std::string texturePath = textureData[TEXTURE_PATH_STRING];
	int textureRows = textureData[ROWS_STRING];
	int textureColumns = textureData[COLUMNS_STRING];

	assert(json.contains(TEXTURE_SIZE_STRING));
	auto textureSize = json[TEXTURE_SIZE_STRING];
	std::array<uint32_t, 2> textureDimension = {textureSize[0], textureSize[1]};

	assert(json.contains(DATA_PATH_STRING));
	std::string dataPath = json[DATA_PATH_STRING];

	std::string fullMapDataPath = MAPS_FOLDER;
	fullMapDataPath.append(dataPath);
	std::ifstream file(fullMapDataPath.c_str());

	std::string str;
	std::vector<int> data;

	while (std::getline(file, str, ','))
	{
		data.emplace_back(std::stoi(str));
	}

	return std::make_unique<MapAttributes>(id, rows, columns, textureDimension, texturePath, textureRows,
	                                       textureColumns, dataPath, data);
}
