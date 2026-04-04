#pragma once
#include "Util/SerialUtil.h"

class Entity;
class World;

struct TerrainData;

class TerrainSystem
{
public:
	void CreateTerrain(World* world, const TerrainData& data) const;
	void DestroyTerrain(const std::string& mapId);

private:
	void _ParseData(const TerrainData& data, Entity& entity) const;
};
