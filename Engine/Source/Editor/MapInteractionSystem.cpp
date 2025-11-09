#include "LLGL/Key.h"

#include "Core/Camera.h"
#include "Graphics/RenderContext.h"
#include "Input/InputHandler.h"
#include "Map/MapRegistry.h"

#include "MapInteractionSystem.h"

MapInteractionSystem::MapInteractionSystem(Camera& camera, InputHandler& inputHandler,
                                           MapRegistry& mapRegistry, RenderContext& renderContext)
	: mRenderContext(renderContext), mMapRegistry(mapRegistry), mCamera(camera)
{
	inputHandler.RegisterMouseMoveHandler([this](LLGL::Offset2D mousePos) { _HandleMouseMove(mousePos); });
	inputHandler.RegisterButtonDownHandler(LLGL::Key::LButton, [this]() { _OnClick(); });
}

// Debug draw intersection point
//DebugDrawManager::GetInstance()->DrawBox(mIntersectionPoint, {.05, .05, .05}, {100, 50, 20});


void MapInteractionSystem::SetPaletteBrush(int brushIndex)
{
	mCurrentBrushIndex = brushIndex;
}

void MapInteractionSystem::_HandleMouseMove(LLGL::Offset2D mousePos)
{
	mMousePos = {mousePos.x, mousePos.y};
}

void MapInteractionSystem::_OnClick()
{
	const auto ray = _CalculateMouseRay(mMousePos);
	// TODO: Switch focus when clicking on map?
	//int i = 0;
	/*
	 *
	for (auto& map : mMapRegistry.GetAllMaps())
	{
		auto& data = map->GetData();
		const auto mapSize = map->GetMapSize();
		const auto position = map->GetPosition();

		const glm::vec3 mapTopLeft = {
			position.x - (mapSize.x / 2),
			position.y + (mapSize.y / 2),
			position.z
		};
		if (const auto t = _WithinMapBounds(mapTopLeft, mapSize, ray); t >= 0)
		{
			//mCurrentFocusedMapIndex = i;
			for (int i = 0; i < data.size(); i++)
			{
				glm::vec3 tilePos;
				glm::vec3 size;
				_CalculateTileInteractionData(*map, map->GetColumns(), i, tilePos, size);

				const auto cameraPos = mCamera.GetPosition();
				const auto intersection = glm::vec3(cameraPos.x + ray.x * t, cameraPos.y + ray.y * t,
				                                    cameraPos.z + ray.z * t);
				if (_Intersect(tilePos, size, intersection))
				{
					map->UpdateTile(i, mCurrentBrushIndex);
				}
			}
		}
		//i++;
	}
	 */
}

glm::vec3 MapInteractionSystem::_CalculateMouseRay(glm::vec2 mousePos) const
{
	const auto normalizeCoords = mRenderContext.NormalizedDeviceCoords({mousePos.x, mousePos.y, 1.0});
	const glm::vec4 homogenousClip = {normalizeCoords.x, normalizeCoords.y, -1.0f, 1.0f};
	glm::vec4 eyeRay = glm::inverse(mRenderContext.GetPerspectiveProjection()) * homogenousClip;
	eyeRay = glm::vec4(eyeRay.x, eyeRay.y, -1.0f, 0.0f);
	const auto ray = glm::normalize(glm::inverse(mCamera.GetView()) * eyeRay);
	return ray;
}

float MapInteractionSystem::_WithinMapBounds(glm::vec3 position, glm::vec3 size, glm::vec3 mouseRay) const
{
	// width/height vectors
	const glm::vec3 p1 = {position.x, position.y + size.y, position.z};
	const glm::vec3 p2 = {position.x + size.x, position.y, position.z};
	const glm::vec3 widthVector = p2 - position;
	const glm::vec3 heightVector = p1 - position;

	// Optionally: we might not have to normalize 
	const auto normal = glm::normalize(cross(widthVector, heightVector));

	const auto unitNormal = normal / glm::length(normal);
	const auto d = -((unitNormal.x * p1.x) + (unitNormal.y * p1.y) + (unitNormal.z * p1.z));

	// ray-plane intersection check
	const auto numerator = glm::dot(mCamera.GetPosition(), normal) + d;
	const auto denominator = glm::dot(mouseRay, normal);
	return -1 * (numerator / denominator);
}

void MapInteractionSystem::_CalculateTileInteractionData(const Map& map, int columns, int tileIndex,
                                                         glm::vec3& pos, glm::vec3& size) const
{
	/*
	 *
	const auto mapSize = map.GetMapSize();
	const auto position = map.GetPosition();
	const auto tileSize = map.GetTileSize();
	const float halfTileSize = static_cast<float>(tileSize) / 2.0f;
	const glm::vec3 mapTopLeft = {
		position.x - (mapSize.x / 2) + halfTileSize,
		position.y + (mapSize.y / 2) - halfTileSize,
		position.z
	};

	const float posX = tileIndex % columns;
	const float currentRow = floorf(tileIndex / columns);

	pos = {
		mapTopLeft.x + (posX * tileSize),
		mapTopLeft.y - (currentRow * tileSize),
		mapTopLeft.z
	};
	// TODO: Draw center of each tile
	//DebugDrawManager::GetInstance()->DrawBox(pos, {0.1f, 0.1f, 1.0f}, {255, 0, 0});
	size = {tileSize, tileSize, 1.0f};
	 */
}

// pos is center of the tile
bool MapInteractionSystem::_Intersect(glm::vec3 pos, glm::vec3 size, glm::vec3 intersection)
{
	const auto minX = pos.x - (size.x / 2);
	const auto maxX = pos.x + (size.x / 2);
	const auto minY = pos.y + (size.y / 2);
	const auto maxY = pos.y - (size.y / 2);

	if (intersection.x > minX && intersection.x < maxX && intersection.y < minY && intersection.y > maxY)
	{
		mIntersectionPoint = intersection;
		return true;
	}

	return false;
}
