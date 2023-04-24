#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "LLGL/PipelineState.h"
#include "LLGL/RenderTarget.h"
#include "LLGL/Texture.h"
#include "MapDescription.h"

class MapDescription;

class Map
{
public:
	Map(const std::string& fileName, std::function<void()> onUpdateCallback);
	Map(const std::string& fileName, glm::vec3 position, glm::vec3 rotation, float tileSize);
	~Map() = default;

	const std::unique_ptr<MapDescription>& GetMapDescription() const;

	glm::vec4 GetClipForTile(int index) const;
	void CalculateTileDrawData(int tileIndex, glm::vec3& pos, glm::vec3& size, glm::vec4& clip) const;

	bool GetRenderDebug() const;
	void SetRenderDebug(bool renderDebug);

	void SaveTilesToFile() const;

	LLGL::Texture& GetMapTexture() const;
	void SetMapTexture(LLGL::Texture* texture);

	LLGL::PipelineState& GetMapTexturePipeline() const;
	void SetMapTexturePipeline(LLGL::PipelineState* state);

	LLGL::RenderTarget& GetMapRenderTarget() const;
	void SetMapRenderTarget(LLGL::RenderTarget* target);

	int GetMapTextureId() const;
	void SetMapTextureId(int id);

	const LLGL::Extent2D& GetMapTextureSize() const;

	void UpdateTile(int tileIndex, int brush) const;

	glm::vec3 GetPosition() const;
	void SetPosition(glm::vec3 pos);
	glm::vec3 GetRotation() const;
	void SetRotation(glm::vec3 pos);

	glm::vec3 GetMapSize() const;
	float GetTileSize() const;

private:
	bool mRenderDebug = false;
	std::unique_ptr<MapDescription> mMapDescription;

	LLGL::PipelineState* mMapTexturePipeline = nullptr;
	LLGL::RenderTarget* mMapRenderTarget = nullptr;
	LLGL::Texture* mMapTexture = nullptr;
	int mMapTextureId = 0;
	LLGL::Extent2D mMapTextureSize = LLGL::Extent2D(0, 0);

	std::function<void()> mOnUpdateCallback;

	glm::vec3 mPosition = {0, 0, 0};
	glm::vec3 mMapSize = {0, 0, 0};
	glm::vec3 mRotation = {0, 0, 0};
	float mTileSize = 1.0f;
};
