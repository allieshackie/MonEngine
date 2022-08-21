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

int TileSetDescription::getTileWidth() const
{
	return mTileWidth;
}

int TileSetDescription::getTileHeight() const
{
	return mTileHeight;
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

    auto size = tileSetJSON[TILESET_STRING][SIZE_STRING];
    mTileWidth = size[0];
    mTileHeight = size[1];

    mTexturePath = tileSetJSON[TILESET_STRING][TEXTURE_STRING];
   
    ifs.close();
}
