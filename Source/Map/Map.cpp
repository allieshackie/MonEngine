#include "Core/ResourceManager.h"
#include "TileSetDescription.h"

#include "Map.h"

Map::Map(ResourceManager& resourceManager, glm::vec3 position, const char* fileName)
	: mMapPosition(position), mResourceManager(resourceManager)
{
	_Load(fileName);
}

void Map::_Load(const char* fileName)
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
			//mTileSetDescription = std::make_unique<TileSetDescription>(str.c_str());
			break;
		case 1:
			mMapRowsColumns.x = std::stof(str);
			break;
		case 2:
			mMapRowsColumns.y = std::stof(str);
			break;
		case 3:
			mMapTileSize = std::stof(str);
			mMapSize = {mMapRowsColumns.y * mMapTileSize, mMapRowsColumns.x * mMapTileSize, 1};
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
	const glm::vec2 mapTopLeft = {
		mMapPosition.x - (mMapSize.x / 2) + (mMapTileSize / 2), mMapPosition.y - (mMapSize.y / 2) + (mMapTileSize / 2)
	};

	for (int i = 0; i < mRawTiles.size(); i++)
	{
		const float posX = i % static_cast<int>(mMapRowsColumns.y);
		const float currentRow = floor(i / mMapRowsColumns.y);
		const auto clip = mTileSetDescription->GetClipForTile(mRawTiles[i]);

		const auto id = "tile-" + std::to_string(i);
		const auto tile = std::make_shared<Tile>(id, i, mRawTiles[i] == -1);

		const glm::vec3 pos = {
			mapTopLeft.x + (posX * mMapTileSize),
			mapTopLeft.y + (currentRow * mMapTileSize),
			mMapPosition.z
		};
		const glm::vec3 size = {mMapTileSize, mMapTileSize, 0};
		mResourceManager.AddSprite(textureName,
		                           pos,
		                           size,
		                           glm::vec2(clip.x, clip.y),
		                           glm::vec2(clip.z, clip.w),
		                           id);
		// Debug draw data
		mResourceManager.AddBox(pos, size, id);

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

const std::vector<std::shared_ptr<Tile>>& Map::GetTiles() const
{
	return mMapTiles;
}

void Map::UpdateTile(int tileIndex, int brushIndex)
{
	const auto clip = mTileSetDescription->GetClipForTile(brushIndex);

	const auto tile = mMapTiles[tileIndex];
	const auto tileDrawData = mResourceManager.GetDrawDataById(tile->GetId());
	tileDrawData->mTexClip = glm::vec2(clip.x, clip.y);
	tileDrawData->mTexScale = glm::vec2(clip.z, clip.w);

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
