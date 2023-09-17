#include "MapAttributes.h"

#include "Core/FileSystem.h"

std::shared_ptr<MapAttributes> MapHelpers::CreateAttributes(const std::string& fileName)
{
	auto attributes = std::make_shared<MapAttributes>();

	const auto json = FileSystem::ReadJson(fileName);
	assert(json.contains(ID_STRING));
	attributes->mId = json[ID_STRING];

	assert(json.contains(ROWS_STRING));
	attributes->mRows = json[ROWS_STRING];
	assert(json.contains(COLUMNS_STRING));
	attributes->mColumns = json[COLUMNS_STRING];

	assert(json.contains(TEXTURE_DATA_STRING));
	const auto textureData = json[TEXTURE_DATA_STRING];
	attributes->mTexturePath = textureData[TEXTURE_PATH_STRING];
	attributes->mTextureMapRows = textureData[ROWS_STRING];
	attributes->mTextureMapColumns = textureData[COLUMNS_STRING];

	assert(json.contains(TEXTURE_SIZE_STRING));
	auto textureSize = json[TEXTURE_SIZE_STRING];
	attributes->mTextureSize = {textureSize[0], textureSize[1]};

	assert(json.contains(DATA_PATH_STRING));
	attributes->mDataPath = json[DATA_PATH_STRING];

	std::ifstream file(attributes->mDataPath.c_str());

	std::string str;

	while (std::getline(file, str, ','))
	{
		attributes->mData.push_back(std::stoi(str));
	}

	return attributes;
}
