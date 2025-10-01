#pragma once
#include "Util/SerialUtil.h"

class Entity;
class World;

struct MapData;

struct TextureData
{
	std::string mTexturePath;
	int mTextureRows = 0;
	int mTextureColumns = 0;

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("texture_path", mTexturePath),
		        cereal::make_nvp("rows", mTextureRows),
		        cereal::make_nvp("columns", mTextureColumns)
		);
	}
};

class MapRegistry
{
public:
	void OpenMap(World* world, const MapData& mapData) const;
	void CloseMap(const std::string& mapId);

private:
	bool _ParseMapData(const MapData& mapData, Entity& entity) const;
};
