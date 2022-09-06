#include "nlohmann/json.hpp"

#include "TileSetDescription.h"

using json = nlohmann::json;

TileSetDescription::TileSetDescription(const char* fileName)
{
    Load(fileName);
}

void TileSetDescription::Load(const char* fileName)
{
    ParseJSON(fileName);
}

const std::string& TileSetDescription::getTexturePath() const
{
    return mTexturePath;
}

int TileSetDescription::GetRows() const
{
    return mTilesetRows;
}

int TileSetDescription::GetColumns() const
{
    return mTilesetColumns;
}

glm::vec4 TileSetDescription::GetClipForTile(int index) const
{
    const float clipAcross = mTextureSize.x / mTilesetColumns;
    const float clipDown = mTextureSize.y / mTilesetRows;
    const int calculateDown = std::floor(index / mTilesetColumns);
    const int calculateAcross = index % mTilesetColumns;
    return glm::vec4(clipAcross * calculateAcross, clipDown * calculateDown, clipAcross, clipDown);
}

void TileSetDescription::SetRows(int rows)
{
    mTilesetRows = rows;
}

void TileSetDescription::setColumns(int columns)
{
    mTilesetColumns = columns;
}

void TileSetDescription::UpdateSize(int rows, int columns)
{
    if (mTileSetFilePath != JSON_PATH)
    {
        mTilesetRows = rows;
        mTilesetColumns = columns;

        mJson[TILESET_STRING][SIZE_STRING][0] = rows;
        mJson[TILESET_STRING][SIZE_STRING][1] = columns;

        std::ofstream ofs(mTileSetFilePath.c_str(), std::ios::out | std::ios::trunc);
        ofs << std::setw(4) << mJson << std::endl;
    }
}

void TileSetDescription::ParseJSON(const char* fileName)
{
    // parse and serialize JSON
    mTileSetFilePath.append(fileName);
    if (mTileSetFilePath.find(".json") == std::string::npos)
    {
        mTileSetFilePath.append(".json");
    }
    std::ifstream ifs(mTileSetFilePath.c_str());

    mJson = json::parse(ifs, nullptr, false, true);

    if (!mJson.contains(TILESET_STRING)) {
        std::cout << "ERROR: TileSet json needs to contain tileset";
        return;
    }

    auto& size = mJson[TILESET_STRING][SIZE_STRING];
    mTilesetRows = size[0];
    mTilesetColumns = size[1];

    mTexturePath = mJson[TILESET_STRING][TEXTURE_STRING];

    const auto& texSize = mJson[TILESET_STRING][TEXTURE_SIZE_STRING];

    if (!texSize.is_null())
    {
       mTextureSize = { texSize[0], texSize[1] };
    }
   
    ifs.close();
}
