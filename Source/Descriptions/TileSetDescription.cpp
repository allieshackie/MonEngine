#include "nlohmann/json.hpp"

#include "TileSetDescription.h"

using json = nlohmann::json;

TileSetDescription::TileSetDescription(const char* fileName)
{
    Load(fileName);
}

void TileSetDescription::Load(const char* fileName)
{
    parseJSON(fileName);
}

const std::string& TileSetDescription::getTexturePath() const
{
    return mTexturePath;
}

glm::vec4 TileSetDescription::GetClipForTile(int index) const
{
    const float clipAcross = mTextureSize.x / mTilesetColumns;
    const float clipDown = mTextureSize.y / mTilesetRows;
    const int calculateDown = std::floor(index / mTilesetColumns);
    const int calculateAcross = index % mTilesetColumns;
    return glm::vec4(clipAcross * calculateAcross, clipDown * calculateDown, clipAcross, clipDown);
}

void TileSetDescription::parseJSON(const char* fileName)
{
    // parse and serialize JSON
    std::string path = JSON_PATH;
    path.append(fileName).append(".json");
    std::ifstream ifs(path.c_str());

    // json parser can't handle comments
    json tileSetJSON = json::parse(ifs);

    if (!tileSetJSON.contains(TILESET_STRING)) {
        std::cout << "ERROR: TileSet json needs to contain tileset";
        return;
    }

    auto& size = tileSetJSON[TILESET_STRING][SIZE_STRING];
    mTilesetRows = size[0];
    mTilesetColumns = size[1];

    mTexturePath = tileSetJSON[TILESET_STRING][TEXTURE_STRING];

    const auto& texSize = tileSetJSON[TILESET_STRING][TEXTURE_SIZE_STRING];

    if (!texSize.is_null())
    {
       mTextureSize = { texSize[0], texSize[1] };
    }
   
    ifs.close();
}
