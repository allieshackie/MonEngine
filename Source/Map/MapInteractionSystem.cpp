#include "LLGL/Key.h"
#include "Camera.h"
#include "Defines.h"
#include "InputManager.h"
#include "MapSystem.h"
#include "Core/Renderer.h"
#include "Core/ResourceManager.h"
#include "Debug/DebugDraw.h"

#include "MapInteractionSystem.h"

MapInteractionSystem::MapInteractionSystem(MapSystem& mapSystem, Renderer& renderer, Camera& camera,
                                           const InputManager& inputManager)
	: mCamera(camera), mRenderer(renderer), mMapSystem(mapSystem)
{
	inputManager.registerMouseMoveHandler([this](LLGL::Offset2D mousePos) { _HandleMouseMove(mousePos); });
	inputManager.registerButtonDownHandler(LLGL::Key::LButton, [this]() { _OnClick(); });
}

void MapInteractionSystem::Tick() const
{
	if (DEBUG_DRAW)
	{
		DebugDrawManager::GetInstance()->DrawBox(mIntersectionPoint, {.05, .05, .05}, {100, 50, 20});
	}
}

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
	for (auto& map : mMapSystem.GetAllMaps())
	{
		const auto pos = map->GetPosition();
		const auto mapSize = map->GetMapSize();
		auto& data = map->GetMapData();

		const glm::vec3 mapTopLeft = {
			pos.x - (mapSize.x / 2),
			pos.y + (mapSize.y / 2),
			pos.z
		};
		if (const auto t = _WithinMapBounds(mapTopLeft, mapSize, ray); t >= 0)
		{
			//mCurrentFocusedMapIndex = i;
			for (int i = 0; i < data.size(); i++)
			{
				glm::vec3 tilePos;
				glm::vec3 size;
				glm::vec4 clip;
				map->CalculateTileDrawData(i, tilePos, size, clip);

				const auto cameraPos = mCamera.GetPosition();
				const auto intersection = glm::vec3(cameraPos.x + ray.x * t, cameraPos.y + ray.y * t,
				                                    cameraPos.z + ray.z * t);
				if (_Intersect(tilePos, size, intersection))
				{
					data[i] = mCurrentBrushIndex;
				}
			}
		}
		//i++;
	}
}

glm::vec3 MapInteractionSystem::_CalculateMouseRay(glm::vec2 mousePos) const
{
	const auto normalizeCoords = mRenderer.NormalizedDeviceCoords({mousePos.x, mousePos.y, 1.0});
	const glm::vec4 homogenousClip = {normalizeCoords.x, normalizeCoords.y, -1.0f, 1.0f};
	glm::vec4 eyeRay = glm::inverse(mRenderer.GetProjection()) * homogenousClip;
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
