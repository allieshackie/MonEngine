#include "LLGL/Key.h"
#include "Camera.h"
#include "InputManager.h"
#include "EntityRegistry.h"
#include "Core/Renderer.h"
#include "Core/ResourceManager.h"
#include "Components/InteractiveComponent.h"
#include "Components/MapComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/TransformComponent.h"
#include "Debug/DebugDraw.h"

#include "MapInteractionService.h"

MapInteractionService::MapInteractionService(Renderer& renderer, Camera& camera, const InputManager& inputManager,
                                             EntityRegistry& entityRegistry)
	: mCamera(camera), mRenderer(renderer), mEntityRegistry(entityRegistry)
{
	inputManager.registerMouseMoveHandler([this](LLGL::Offset2D mousePos) { _HandleMouseMove(mousePos); });
	inputManager.registerButtonDownHandler(LLGL::Key::LButton, [this]() { _OnClick(); });
}

void MapInteractionService::Tick() const
{
	DebugDrawManager::GetInstance()->DrawBox(mIntersectionPoint, 0.1f, {255, 255, 255});
}

void MapInteractionService::_HandleMouseMove(LLGL::Offset2D mousePos)
{
	mMousePos = {mousePos.x, mousePos.y};
}

void MapInteractionService::_OnClick()
{
	const auto ray = _CalculateMouseRay(mMousePos);
	const auto interactableView = mEntityRegistry.GetEnttRegistry().view<
		const TransformComponent, const SpriteComponent, const MapComponent, const InteractiveComponent>();
	interactableView.each([=, &ray](const TransformComponent& transform, const SpriteComponent& sprite,
	                                const MapComponent& map)
	{
		const glm::vec2 mapTopLeft = {
			transform.mPosition.x - (map.mMapSize.x / 2) + (map.mTileSize / 2),
			transform.mPosition.y - (map.mMapSize.y / 2) + (map.mTileSize / 2)
		};
		auto model = glm::mat4(1.0f);
		model = translate(model, transform.mPosition);
		model = translate(model, glm::vec3(0.5f * transform.mSize.x, 0.5f * transform.mSize.y, 0.0f));
		model = rotate(model, glm::radians(transform.mRotation), glm::vec3(0.0f, 0.0f, 1.0f));
		model = translate(model, glm::vec3(-0.5f * transform.mSize.x, -0.5f * transform.mSize.y, 0.0f));
		model = scale(model, transform.mSize);
		if (const auto t = _WithinMapBounds(model, ray); t >= 0)
		{
			for (int i = 0; i < map.mData.size(); i++)
			{
				const float posX = i % static_cast<int>(map.mColumns);
				const double currentRow = floor(i / map.mColumns);

				const glm::vec3 pos = {
					mapTopLeft.x + (posX * map.mTileSize),
					mapTopLeft.y + (currentRow * map.mTileSize),
					transform.mPosition.z
				};
				const glm::vec3 size = {map.mTileSize, map.mTileSize, 0};

				if (_Intersect(pos, size, ray * t))
				{
					std::cout << "Hovered tile: " << i << std::endl;
					//mCurrentSelectedObject = obj;
				}
			}
		}
	});
}

glm::vec3 MapInteractionService::_CalculateMouseRay(glm::vec2 mousePos) const
{
	const auto normalizeCoords = mRenderer.NormalizedDeviceCoords({mousePos.x, mousePos.y, 1.0});
	const glm::vec4 homogenousClip = {normalizeCoords.x, normalizeCoords.y, -1.0f, 1.0f};
	glm::vec4 eyeRay = glm::inverse(mRenderer.GetProjection()) * homogenousClip;
	eyeRay = glm::vec4(eyeRay.x, eyeRay.y, -1.0f, 1.0f);
	return glm::normalize(glm::inverse(mCamera.GetView()) * eyeRay);
}

float MapInteractionService::_WithinMapBounds(glm::mat4 model, glm::vec3 mouseRay) const
{
	// plane value
	const glm::vec3 position = model * glm::vec4(vertices[0].position, 1.0);

	// width/height vectors
	const glm::vec3 p1 = model * glm::vec4(vertices[1].position, 1.0);
	const glm::vec3 p2 = model * glm::vec4(vertices[2].position, 1.0);
	const glm::vec3 widthVector = p2 - position;
	const glm::vec3 heightVector = p1 - position;

	// Optionally: we might not have to normalize 
	const auto normal = glm::normalize(cross(widthVector, heightVector));

	const auto unitNormal = normal / glm::length(normal);
	const auto d = -((unitNormal.x * p1.x) + (unitNormal.y * p1.y) + (unitNormal.z * p1.z));

	// ray-plane intersection check
	const auto numerator = glm::dot(mCamera.GetPosition(), normal) + d;
	const auto denominator = glm::dot(mouseRay, normal);
	return numerator / denominator;
}

bool MapInteractionService::_Intersect(glm::vec3 pos, glm::vec3 size, glm::vec3 intersection)
{
	const glm::vec3 position = pos;
	// width/height vectors
	const glm::vec3 p1 = {pos.x, pos.y + size.y, pos.z};
	const glm::vec3 p2 = {pos.x + size.x, pos.y, pos.z};
	const glm::vec3 widthVector = p2 - position;
	const glm::vec3 heightVector = p1 - position;

	const auto width = glm::length(widthVector);
	const auto height = glm::length(heightVector);

	const auto proj1 = glm::dot(intersection, widthVector) / width;
	const auto proj2 = glm::dot(intersection, heightVector) / height;

	if ((proj1 < width && proj1 > 0) && (proj2 < height && proj2 > 0))
	{
		mIntersectionPoint = intersection;
		std::cout << "Interaction: " << mIntersectionPoint.x << " " << mIntersectionPoint.y << " " << mIntersectionPoint
			.z
			<< std::endl;
		return true;
	}


	return false;
}
