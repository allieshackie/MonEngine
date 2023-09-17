#include <glm/vec2.hpp>
#include "MapAttributes.h"

#include "Map.h"

Map::Map(const std::string& fileName) : mMapId(fileName)
{
	mMapAttributes = MapHelpers::CreateAttributes(fileName);

	mMapSize = {
		static_cast<float>(mMapAttributes->mColumns) * mTileSize,
		static_cast<float>(mMapAttributes->mRows) * mTileSize, 1
	};
}

Map::Map(const std::string& fileName, glm::vec3 position, glm::vec3 rotation, float tileSize) : mMapId(fileName),
	mPosition(position), mRotation(rotation), mTileSize(tileSize)
{
	mMapAttributes = MapHelpers::CreateAttributes(fileName);

	mMapSize = {
		static_cast<float>(mMapAttributes->mColumns) * mTileSize,
		static_cast<float>(mMapAttributes->mRows) * mTileSize, 1
	};
}

const std::string& Map::GetMapId() const
{
	return mMapId;
}

glm::vec3 Map::GetPosition() const
{
	return mPosition;
}

glm::vec3 Map::GetRotation() const
{
	return mRotation;
}

glm::vec3 Map::GetMapSize() const
{
	return mMapSize;
}

float Map::GetTileSize() const
{
	return mTileSize;
}

int Map::GetMapTextureId() const
{
	return mMapTextureId;
}

int Map::GetRows() const
{
	return mMapAttributes->mRows;
}

int Map::GetColumns() const
{
	return mMapAttributes->mColumns;
}

const std::vector<int>& Map::GetData() const
{
	return mMapAttributes->mData;
}

int Map::GetMapTextureRows() const
{
	return mMapAttributes->mTextureMapRows;
}

int Map::GetMapTextureColumns() const
{
	return mMapAttributes->mTextureMapColumns;
}

const std::string& Map::GetTexturePath() const
{
	return mMapAttributes->mTexturePath;
}

void Map::SetMapTextureId(int id)
{
	mMapTextureId = id;
}

void Map::UpdateTile(int tileIndex, int brush) const
{
	mMapAttributes->mData[tileIndex] = brush;
}


void Map::SetPosition(glm::vec3 pos)
{
	mPosition = pos;
}


void Map::SetRotation(glm::vec3 rot)
{
	mRotation = rot;
}

void Map::SetTextureRows(int rows) const
{
	mMapAttributes->mTextureMapRows = rows;
}

void Map::SetTextureColumns(int columns) const
{
	mMapAttributes->mTextureMapColumns = columns;
}


glm::vec4 Map::GetClipForTile(int index) const
{
	const auto texColumns = mMapAttributes->mTextureMapColumns;
	const auto texRows = mMapAttributes->mTextureMapRows;
	const auto whichColumn = fmod(index, texColumns);
	// Add one or else the last index in row tries to go to next row
	const auto whichRow = floor(index / (texRows + 1));
	const glm::vec2 clipStart = {
		whichColumn / texColumns, whichRow / texRows
	};
	return {
		clipStart.x, clipStart.y, 1.0 / texColumns, 1.0 / texRows
	};
}

void Map::CalculateTileDrawData(int tileIndex, glm::vec3& pos, glm::vec3& size, glm::vec4& clip) const
{
	const auto rows = mMapAttributes->mRows;
	const auto columns = mMapAttributes->mColumns;
	const glm::vec2 tileSize = {(1.0f / columns) * 2, (1.0f / rows) * 2};
	const glm::vec3 mapTopLeft = {-1 + (tileSize.x / 2.0f), 1 - (tileSize.y / 2.0f), 0};

	const int tile = mMapAttributes->mData[tileIndex];
	const float posX = tileIndex % columns;
	const float currentRow = floorf(tileIndex / columns);

	clip = GetClipForTile(tile);

	pos = {
		mapTopLeft.x + (posX * tileSize.x),
		mapTopLeft.y - (currentRow * tileSize.y),
		mapTopLeft.z
	};
	size = {tileSize.x, tileSize.y, 0};
}

void Map::SaveTilesToFile() const
{
	const auto& data = mMapAttributes->mData;

	std::ofstream file;
	file.open(mMapAttributes->mDataPath, std::ofstream::out | std::ofstream::trunc);

	for (int i = 0; i < data.size(); i++)
	{
		file << data[i];
		if (i < data.size() - 1)
		{
			file << ",";
		}
	}
}
