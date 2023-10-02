#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "MapAttributes.h"

class Map
{
public:
	Map(const std::string& fileName);
	Map(const std::string& fileName, glm::vec3 position, glm::vec3 rotation, float tileSize);

	const std::string& GetMapId() const;
	glm::vec3 GetPosition() const;
	glm::vec3 GetRotation() const;
	glm::vec3 GetMapSize() const;
	float GetTileSize() const;
	int GetMapTextureId() const;
	// Accessors for MapAttributes
	int GetRows() const;
	int GetColumns() const;
	const std::vector<int>& GetData() const;
	int GetMapTextureRows() const;
	int GetMapTextureColumns() const;
	// TODO: This should probably be from a Sprite Component
	const std::string& GetTexturePath() const;

	// TODO: Should these be in editor specific class?
	void UpdateTile(int tileIndex, int brush) const;
	void SetMapTextureId(int id);
	void SetPosition(glm::vec3 pos);
	void SetRotation(glm::vec3 rot);
	void SetTextureRows(int rows) const;
	void SetTextureColumns(int columns) const;

	// TODO: These are used for rendering and editing specifically
	// Could probably move them
	glm::vec4 GetClipForTile(int index) const;
	void CalculateTileDrawData(int tileIndex, glm::vec3& pos, glm::vec3& size, glm::vec4& clip) const;
	void SaveTilesToFile() const;

private:
	const std::string& mMapId;

	std::unique_ptr<MapAttributes> mMapAttributes;

	int mMapTextureId = 0;

	glm::vec3 mPosition = {0, 0, 0};
	glm::vec3 mMapSize = {0, 0, 0};
	glm::vec3 mRotation = {0, 0, 0};
	float mTileSize = 1.0f;
};
