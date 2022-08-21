#include "nlohmann/json.hpp"

#include "MapDescription.h"
using json = nlohmann::json;

MapDescription::MapDescription(const char* fileName)
{
    Load(fileName);
}

void MapDescription::Load(const char* fileName)
{
    parseJSON(fileName);
}

int MapDescription::GetMapWidth() const
{
    return mWidth;
}

int MapDescription::GetMapHeight() const
{
    return mHeight;
}

const std::string& MapDescription::GetRawTiles()
{
    return mTiles;
}

const std::string& MapDescription::GetTilesetTexture()
{
    return mTilesetDescription->getTexturePath();
}

void MapDescription::parseJSON(const char* fileName)
{
    // parse and serialize JSON
	std::string path = JSON_PATH;
    path.append(fileName).append(".json");
    std::ifstream ifs(path.c_str());

    // json parser can't handle comments
    json mapJSON = json::parse(ifs);

    if (!mapJSON.contains(MAP_STRING)) {
        std::cout << "ERROR: Map json needs to contain map";
        return;
    }


    if (!mapJSON[MAP_STRING].contains(TILESET_STRING)) {
        std::cout << "ERROR: Map json needs to contain tileset";
        return;
    }

    const std::string tilesetFile = mapJSON[MAP_STRING][TILESET_STRING];
    mTilesetDescription = new TileSetDescription(tilesetFile.c_str());

    if (!mapJSON[MAP_STRING].contains(FILE_STRING)) {
        std::cout << "ERROR: Map json needs to contain text file";
        return;
    }

    mMapTextFile = mapJSON[MAP_STRING][FILE_STRING];

    if (!mapJSON[MAP_STRING].contains(SIZE_STRING)) {
        std::cout << "ERROR: Map json needs to contain size";
        return;
    }

    auto size = mapJSON[MAP_STRING][SIZE_STRING];

    mWidth = size[0];
    mHeight = size[1];

    ifs.close();


    // parse and serialize JSON
    std::string mapTextPath = JSON_PATH;
    mapTextPath.append(mMapTextFile).append(".txt");
    std::ifstream textfs(mapTextPath.c_str());

    if (textfs.is_open())
    {
        std::string content((std::istreambuf_iterator<char>(textfs)), std::istreambuf_iterator<char>());
        mTiles = content;
      
    	textfs.close();
    }
}
