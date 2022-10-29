#include "Core/ResourceManager.h"
#include "TileSetDescription.h"
#include "Tile.h"

#include "Map.h"

Map::Map(glm::vec3 position) : RenderObject(position, {1, 1, 1}), mMapPosition(position)
{
}

void Map::Draw()
{
	for (const auto tile : mMapTiles)
	{
		tile->Draw();
	}
}


void Map::Load(const char* fileName)
{
	_ReadFile(fileName);
	_CreateTiles();
}

void Map::_ReadFile(const char* fileName)
{
	mMapPath = MAP_PATH;
	mMapPath.append(fileName);
	if (mMapPath.find(".txt") == std::string::npos)
	{
		mMapPath.append(".txt");
	}
	std::ifstream file(mMapPath);

	std::string str;
	int lineIndex = 0;

	while (std::getline(file, str, ','))
	{
		switch (lineIndex)
		{
		case 0:
			mTileSetDescription = std::make_unique<TileSetDescription>(str.c_str());
			break;

		case 1:
			mMapRowsColumns.x = std::stof(str);
			break;
		case 2:
			mMapRowsColumns.y = std::stof(str);
			break;
		case 3:
			mMapTileSize = std::stoi(str);
			mMapSize = {mMapRowsColumns.y * mMapTileSize, mMapRowsColumns.x * mMapTileSize, 0};
			break;
		default:
			mRawTiles.push_back(std::stoi(str));
		}

		lineIndex++;
	}
}

void Map::_CreateTiles()
{
	const auto textureName = mTileSetDescription->getTexturePath();

	for (int i = 0; i < mRawTiles.size(); i++)
	{
		const float posX = i % static_cast<int>(mMapRowsColumns.y);
		const float currentRow = floor(i / mMapRowsColumns.y);
		const auto clip = mTileSetDescription->GetClipForTile(mRawTiles[i]);
		const auto tile = ResourceManager::GetInstance()->CreateTile(
			textureName,
			{mMapPosition.x + (posX * mMapTileSize), mMapPosition.y + (currentRow * mMapTileSize), 0},
			{mMapTileSize, mMapTileSize, 0}, glm::vec2(clip.x, clip.y), glm::vec2(clip.z, clip.w));

		// -1 is an empty tile
		if (mRawTiles[i] == -1)
		{
			tile->SetEmpty(true);
		}

		mMapTiles.push_back(tile);
	}
}

glm::vec3 Map::GetMapPosition() const
{
	return mMapPosition;
}

glm::vec2 Map::GetMapRowsColumns() const
{
	return mMapRowsColumns;
}

glm::vec3 Map::GetMapSize() const
{
	return mMapSize;
}

int Map::GetTotalTileSetTiles() const
{
	return mTileSetDescription->GetTotalTiles();
}

const std::string& Map::getTexturePath() const
{
	return mTileSetDescription->getTexturePath();
}

glm::vec4 Map::GetClipForTile(int index) const
{
	return mTileSetDescription->GetClipForTile(index);
}

const std::vector<Tile*>& Map::GetTiles() const
{
	return mMapTiles;
}

void Map::UpdateTile(int tileIndex, int brushIndex)
{
	const float posX = tileIndex % static_cast<int>(mMapRowsColumns.y);
	const float currentRow = floor(tileIndex / mMapRowsColumns.y);
	const auto clip = mTileSetDescription->GetClipForTile(brushIndex);

	mMapTiles[tileIndex] = ResourceManager::GetInstance()->CreateTile(
		mTileSetDescription->getTexturePath(),
		{mMapPosition.x + (posX * mMapTileSize), mMapPosition.y + (currentRow * mMapTileSize), 0},
		{mMapTileSize, mMapTileSize, 0}, glm::vec2(clip.x, clip.y), glm::vec2(clip.z, clip.w));

	mRawTiles[tileIndex] = brushIndex;
}

void Map::SaveTilesToFile() const
{
	std::ifstream file(mMapPath);

	std::string strTemp;
	std::getline(file, strTemp);
	file.close();

	std::ofstream fileOut(mMapPath, std::ios::trunc);
	// write first line directly to outfile (tileset name, rows, columns, tilesize)
	fileOut << strTemp << "\n";

	strTemp.clear();
	int columnIndex = 0;
	for (int i = 0; i < mRawTiles.size(); i++)
	{
		strTemp += std::to_string(mRawTiles[i]);
		strTemp += ',';
		columnIndex++;

		if (columnIndex != 0 && (static_cast<int>(mMapRowsColumns.y) / (columnIndex + 1)) == 0)
		{
			if (i != mRawTiles.size() - 1)
			{
				strTemp += "\n";
			}
			columnIndex = 0;
			fileOut << strTemp;
			strTemp.clear();
		}
	}

	fileOut.close();
}
