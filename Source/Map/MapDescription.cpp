#include "pch.h"

#include "nlohmann/json.hpp"

#include "MapDescription.h"
using json = nlohmann::json;

MapDescription::MapDescription()
{
    parseJSON();
}

const std::vector<std::string>& MapDescription::getTiles()
{
    return mTiles;
}

int MapDescription::getMapWidth()
{
	return mWidth;
}

void MapDescription::parseJSON()
{
    // parse and serialize JSON
    std::ifstream ifs(JSON_PATH);

    // json parser can't handle comments
    json mapJSON = json::parse(ifs);

    if (!mapJSON.contains(MAP_STRING)) {
        std::cout << "ERROR: Map json needs to contain map";
        return;
    }

    if (!mapJSON[MAP_STRING].contains(WIDTH_STRING)) {
        std::cout << "ERROR: Map json needs to contain width";
        return;
    }

    mWidth = mapJSON[MAP_STRING][WIDTH_STRING];

    if (!mapJSON[MAP_STRING].contains(HEIGHT_STRING)) {
        std::cout << "ERROR: Map json needs to contain height";
        return;
    }

    mHeight = mapJSON[MAP_STRING][HEIGHT_STRING];

    if (!mapJSON[MAP_STRING].contains(TILES_STRING)) {
        std::cout << "ERROR: Map json needs to contain tiles";
        return;
    }

    for (json::iterator it = mapJSON[MAP_STRING][TILES_STRING].begin(); it != mapJSON[MAP_STRING][TILES_STRING].end(); ++it) {
        mTiles.push_back(it.value());
    }

    ifs.close();
}
