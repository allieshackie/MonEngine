#include "nlohmann/json.hpp"

#include "TileSetDescription.h"

using json = nlohmann::json;

TileSetDescription::TileSetDescription()
{
    parseJSON();
}

const std::string& TileSetDescription::getTexturePath() const
{
    return mTexturePath;
}

int TileSetDescription::getTileWidth() const
{
	return mTileWidth;
}

int TileSetDescription::getTileHeight() const
{
	return mTileHeight;
}

std::vector<int> TileSetDescription::getTileClipPosition(const std::string& tileId)
{
    for (const auto& tile : mTilesData) {
        if (tile.mId == tileId) {
            return { tile.mClipPosX, tile.mClipPosY };
        }
    }

    return { 0, 0 };
}

void TileSetDescription::parseJSON()
{
    // parse and serialize JSON
    std::ifstream ifs(JSON_PATH);

    // json parser can't handle comments
    json tileSetJSON = json::parse(ifs);

    if (!tileSetJSON.contains(TILESET_STRING)) {
        std::cout << "ERROR: TileSet json needs to contain tileset";
        return;
    }

    if (!tileSetJSON.contains(TILE_DATA_STRING)) {
        std::cout << "ERROR: TileSet json needs to contain tiledata";
        return;
    }

    mTileWidth = tileSetJSON[TILE_DATA_STRING][WIDTH_STRING];
    mTileHeight = tileSetJSON[TILE_DATA_STRING][HEIGHT_STRING];

    if (!tileSetJSON.contains(TILES_STRING)) {
        std::cout << "ERROR: TileSet json needs to contain tiledata";
        return;
    }
    
    for (json::iterator it = tileSetJSON[TILES_STRING].begin(); it != tileSetJSON[TILES_STRING].end(); ++it) {
        const std::vector<int> tileClipPos = it.value();
        mTilesData.push_back({ it.key(), tileClipPos[0], tileClipPos[1] });
    }


    if (!tileSetJSON[TILE_DATA_STRING].contains(TEXTURE_STRING)) {
        std::cout << "ERROR: Map json needs to contain texture";
        return;
    }

    mTexturePath = tileSetJSON[TILE_DATA_STRING][TEXTURE_STRING];
   
    ifs.close();
}
