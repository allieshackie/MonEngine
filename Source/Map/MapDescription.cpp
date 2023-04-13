#include "Defines.h"
#include "Debug/DebugDraw.h"

#include "MapDescription.h"

MapDescription::MapDescription(const std::string& fileName)
{
	// parse and serialize JSON
	std::string fullFileName = MonDev::MAP_PATH;
	fullFileName.append(fileName);

	std::ifstream ifs(fullFileName.c_str());

	const auto json = nlohmann::json::parse(ifs, nullptr, false, true);
	_ParseJSON(json);
}

void MapDescription::_ParseJSON(const nlohmann::json& json)
{
	assert(json.contains(ID_STRING));
	mId = json[ID_STRING];

	assert(json.contains(ROWS_STRING));
	mRows = json[ROWS_STRING];
	assert(json.contains(COLUMNS_STRING));
	mColumns = json[COLUMNS_STRING];

	assert(json.contains(TEXTURE_DATA_STRING));
	const auto textureData = json[TEXTURE_DATA_STRING];
	mTexturePath = textureData[TEXTURE_PATH_STRING];
	mTextureMapRows = textureData[ROWS_STRING];
	mTextureMapColumns = textureData[COLUMNS_STRING];

	assert(json.contains(TEXTURE_SIZE_STRING));
	auto textureSize = json[TEXTURE_SIZE_STRING];
	mTextureSize = {textureSize[0], textureSize[1]};

	assert(json.contains(DATA_PATH_STRING));
	mDataPath = json[DATA_PATH_STRING];

	_ReadFile(mDataPath.c_str());
}

void MapDescription::_ReadFile(const char* fileName)
{
	std::string mapPath = MonDev::MAP_PATH;
	mapPath.append(fileName);

	std::ifstream file(mapPath);

	std::string str;

	while (std::getline(file, str, ','))
	{
		mData.push_back(std::stoi(str));
	}
}

void MapDescription::SetTextureMapRows(int rows)
{
	mTextureMapRows = rows;
}

void MapDescription::SetTextureMapColumns(int columns)
{
	mTextureMapColumns = columns;
}
