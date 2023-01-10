#pragma once
#include "RenderObjects/RenderObject.h"

struct TileSetDescription;

class ResourceManager;
class Tile;

static constexpr char MAP_PATH[] = "../Data/Maps/";

class Map : public RenderObject
{
public:
	Map(ResourceManager& resourceManager, glm::vec3 position, const char* fileName);
	~Map() override = default;

	void Draw(const Renderer& renderer, LLGL::CommandBuffer& commands) override;

	glm::vec3 GetMapPosition() const;
	glm::vec2 GetMapRowsColumns() const;
	glm::vec3 GetMapSize() const;
	int GetTotalTileSetTiles() const;

	const std::string& getTexturePath() const;
	glm::vec4 GetClipForTile(int index) const;

	const std::vector<Tile*>& GetTiles() const;

	void UpdateTile(int tileIndex, int brushIndex);

	void SaveTilesToFile() const;
private:
	void _Load(const char* fileName);
	void _ReadFile(const char* fileName);
	void _CreateTiles();

	std::vector<int> mRawTiles;
	std::vector<Tile*> mMapTiles;

	std::string mMapPath;
	glm::vec3 mMapPosition = {0, 0, 0};
	glm::vec2 mMapRowsColumns = {0, 0};
	glm::vec3 mMapSize = {0, 0, 1};
	int mMapTileSize = 0;

	std::unique_ptr<TileSetDescription> mTileSetDescription;

	ResourceManager& mResourceManager;
};
