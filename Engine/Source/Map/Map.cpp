#include <glm/vec2.hpp>
#include "Core/Defines.h"

#include "Map.h"

Map::Map(const std::string& fileName, std::function<void()> onUpdateCallback)
{
	mMapDescription = std::make_unique<MapDescription>(fileName);
	mOnUpdateCallback = std::move(onUpdateCallback);

	mMapTextureSize = {mMapDescription->GetTextureSize()[0], mMapDescription->GetTextureSize()[1]};
	mMapSize = {
		static_cast<float>(mMapDescription->GetColumns()) * mTileSize,
		static_cast<float>(mMapDescription->GetRows()) * mTileSize, 1
	};
}

Map::Map(const std::string& fileName, glm::vec3 position, glm::vec3 rotation, float tileSize) : mPosition(position),
	mRotation(rotation), mTileSize(tileSize)
{
	mMapDescription = std::make_unique<MapDescription>(fileName);
	mMapTextureSize = {mMapDescription->GetTextureSize()[0], mMapDescription->GetTextureSize()[1]};

	mMapSize = {
		static_cast<float>(mMapDescription->GetColumns()) * tileSize,
		static_cast<float>(mMapDescription->GetRows()) * tileSize, 1
	};
}

const std::unique_ptr<MapDescription>& Map::GetMapDescription() const
{
	return mMapDescription;
}

glm::vec4 Map::GetClipForTile(int index) const
{
	const auto texColumns = mMapDescription->GetTextureMapColumns();
	const auto texRows = mMapDescription->GetTextureMapRows();
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
	const auto rows = mMapDescription->GetRows();
	const auto columns = mMapDescription->GetColumns();
	const glm::vec2 tileSize = {(1.0f / columns) * 2, (1.0f / rows) * 2};
	const glm::vec3 mapTopLeft = {-1 + (tileSize.x / 2.0f), 1 - (tileSize.y / 2.0f), 0};

	const int tile = mMapDescription->GetMapData()[tileIndex];
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

bool Map::GetRenderDebug() const
{
	return mRenderDebug;
}

void Map::SetRenderDebug(bool renderDebug)
{
	mRenderDebug = renderDebug;
}

void Map::SaveTilesToFile() const
{
	const auto& data = mMapDescription->GetMapData();

	std::ofstream file;
	file.open(mMapDescription->GetDataPath(), std::ofstream::out | std::ofstream::trunc);

	for (int i = 0; i < data.size(); i++)
	{
		file << data[i];
		if (i < data.size() - 1)
		{
			file << ",";
		}
	}
}

LLGL::Texture& Map::GetMapTexture() const
{
	return *mMapTexture;
}

void Map::SetMapTexture(LLGL::Texture* texture)
{
	mMapTexture = texture;
}

LLGL::PipelineState& Map::GetMapTexturePipeline() const
{
	return *mMapTexturePipeline;
}

void Map::SetMapTexturePipeline(LLGL::PipelineState* state)
{
	mMapTexturePipeline = state;
}

LLGL::RenderTarget& Map::GetMapRenderTarget() const
{
	return *mMapRenderTarget;
}

void Map::SetMapRenderTarget(LLGL::RenderTarget* target)
{
	mMapRenderTarget = target;
}

int Map::GetMapTextureId() const
{
	return mMapTextureId;
}

void Map::SetMapTextureId(int id)
{
	mMapTextureId = id;
}

const LLGL::Extent2D& Map::GetMapTextureSize() const
{
	return mMapTextureSize;
}

void Map::UpdateTile(int tileIndex, int brush) const
{
	mMapDescription->GetMapData()[tileIndex] = brush;
	mOnUpdateCallback();
}

glm::vec3 Map::GetPosition() const
{
	return mPosition;
}

void Map::SetPosition(glm::vec3 pos)
{
	mPosition = pos;
}

glm::vec3 Map::GetRotation() const
{
	return mRotation;
}

void Map::SetRotation(glm::vec3 rot)
{
	mRotation = rot;
}

glm::vec3 Map::GetMapSize() const
{
	return mMapSize;
}

float Map::GetTileSize() const
{
	return mTileSize;
}
