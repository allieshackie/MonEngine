#pragma once
#include "Util/SerialUtil.h"

class EngineContext;
class Map;
class PhysicsSystem;

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

struct TempMapReg
{
	std::string mId;
	int mRows = 0;
	int mColumns = 0;
	glm::vec2 mTextureSize = {0, 0};
	TextureData mTextureData;
	std::string mDataPath;

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("id", mId),
		        cereal::make_nvp("rows", mRows),
		        cereal::make_nvp("columns", mColumns),
		        cereal::make_nvp("texture_size", mTextureSize),
		        cereal::make_nvp("texture_data", mTextureData),
		        cereal::make_nvp("data_path", mDataPath)
		);
	}
};

class MapRegistry
{
public:
	void OpenMap(const EngineContext& context, const MapData& mapData) const;
	void CloseMap(const std::string& mapId);

private:
	bool _ParseMapData(const MapData& mapData, EntityId entityId, EntityRegistry& entityReg) const;
};
