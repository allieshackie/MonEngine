#pragma once

class EngineContext;
class Map;
class PhysicsSystem;

struct MapData;

class MapRegistry
{
public:
	void OpenMap(const EngineContext& context, const MapData& mapData) const;
	void CloseMap(const std::string& mapId);

private:
	bool _ParseMapData(const MapData& mapData, EntityId entityId, EntityRegistry& entityReg) const;

	static constexpr char ID_STRING[] = "id";

	static constexpr char ROWS_STRING[] = "rows";
	static constexpr char COLUMNS_STRING[] = "columns";
	static constexpr char TEXTURE_SIZE_STRING[] = "texture_size";

	static constexpr char TEXTURE_DATA_STRING[] = "texture_data";
	static constexpr char TEXTURE_PATH_STRING[] = "texture_path";
	static constexpr char DATA_PATH_STRING[] = "data_path";
};
