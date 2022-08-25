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

const std::vector<int>& MapDescription::GetTiles()
{
    return mTiles;
}

const std::string& MapDescription::GetTilesetTexture()
{
    return mTilesetDescription->getTexturePath();
}

glm::vec4 MapDescription::GetClipForTile(int index) const
{
    return mTilesetDescription->GetClipForTile(index);
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
    std::string line;

    while (std::getline(textfs, line))
    {
        std::size_t prev = 0, pos;
        while ((pos = line.find_first_of(" ", prev)) != std::string::npos)
        {
            if (pos > prev)
            {
                mTiles.push_back(std::stoi(line.substr(prev, pos - prev)));
            }
            prev = pos + 1;
        }
        if (prev < line.length())
        {
            mTiles.push_back(std::stoi(line.substr(prev, std::string::npos)));
        }
    }

	textfs.close();
}
