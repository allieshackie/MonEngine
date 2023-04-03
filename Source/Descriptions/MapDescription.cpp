#include "Defines.h"

#include "MapDescription.h"

#include "Debug/DebugDraw.h"

MapDescription::MapDescription(const std::string& fileName)
{
	// parse and serialize JSON
	std::string fullFileName = MAP_PATH;
	fullFileName.append(fileName);

	std::ifstream ifs(fullFileName.c_str());

	auto json = nlohmann::json::parse(ifs, nullptr, false, true);
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
	assert(json.contains(TILE_SIZE_STRING));
	mTileSize = json[TILE_SIZE_STRING];

	assert(json.contains(TRANSFORM_STRING));
	const auto transform = json[TRANSFORM_STRING];
	auto position = transform[POSITION_STRING];
	mPosition = glm::vec3(position[0], position[1], position[2]);

	auto size = transform[SIZE_STRING];
	mSize = glm::vec3(size[0], size[1], size[2]);
	mRotation = transform[ROTATION_STRING];

	assert(json.contains(TEXTURE_DATA_STRING));
	const auto textureData = json[TEXTURE_DATA_STRING];
	mTexturePath = textureData[TEXTURE_PATH_STRING];
	mTextureMapRows = textureData[ROWS_STRING];
	mTextureMapColumns = textureData[COLUMNS_STRING];

	assert(json.contains(DATA_PATH_STRING));
	mDataPath = json[DATA_PATH_STRING];

	mMapSize = {mColumns * mTileSize, mRows * mTileSize, 1};
	_ReadFile(mDataPath.c_str());
}

void MapDescription::_ReadFile(const char* fileName)
{
	std::string mapPath = MAP_PATH;
	mapPath.append(fileName);

	std::ifstream file(mapPath);

	std::string str;

	while (std::getline(file, str, ','))
	{
		mData.push_back(std::stoi(str));
	}
}

void MapDescription::SetPosition(glm::vec3 pos)
{
	mPosition = pos;
}

void MapDescription::SetRotation(float rotation)
{
	mRotation = rotation;
}

void MapDescription::SetTextureMapRows(int rows)
{
	mTextureMapRows = rows;
}

void MapDescription::SetTextureMapColumns(int columns)
{
	mTextureMapColumns = columns;
}

void MapDescription::SetRenderDebug(bool renderDebug)
{
	mRenderDebug = renderDebug;
}

glm::vec4 MapDescription::GetClipForTile(int index) const
{
	const auto whichColumn = fmod(index, mTextureMapColumns);
	// Add one or else the last index in row tries to go to next row
	const auto whichRow = floor(index / (mTextureMapRows + 1));
	const glm::vec2 clipStart = {
		whichColumn / mTextureMapColumns, whichRow / mTextureMapRows
	};
	return {
		clipStart.x, clipStart.y, 1.0 / mTextureMapColumns, 1.0 / mTextureMapRows
	};
}

void MapDescription::CalculateTileDrawData(int tileIndex, glm::vec3& pos, glm::vec3& size, glm::vec4& clip)
{
	const float halfTileSize = static_cast<float>(mTileSize) / 2.0f;
	const glm::vec3 mapTopLeft = {
		mPosition.x - (mMapSize.x / 2) + halfTileSize,
		mPosition.y + (mMapSize.y / 2) - halfTileSize,
		mPosition.z
	};

	const int tile = mData[tileIndex];
	const float posX = tileIndex % mColumns;
	const float currentRow = floorf(tileIndex / mColumns);

	clip = GetClipForTile(tile);


	pos = {
		mapTopLeft.x + (posX * mTileSize),
		mapTopLeft.y - (currentRow * mTileSize),
		mapTopLeft.z
	};
	// TODO: Draw center of each tile
	//DebugDrawManager::GetInstance()->DrawBox(pos, {0.1f, 0.1f, 1.0f}, {255, 0, 0});
	size = {mTileSize, mTileSize, 1.0f};
}

void MapDescription::SaveTilesToFile() const
{
	std::string mapPath = MAP_PATH;
	mapPath.append(mDataPath);

	std::ofstream file;
	file.open(mapPath, std::ofstream::out | std::ofstream::trunc);

	for (int i = 0; i < mData.size(); i++)
	{
		file << mData[i];
		if (i < mData.size() - 1)
		{
			file << ",";
		}
	}
}
