#pragma once
#include "LLGL/Types.h"
#include <glm/vec3.hpp>

#include "Core/Vertex.h"

class Camera;
class DrawData;
class EntityRegistry;
class InputManager;
class Renderer;

class MapInteractionService
{
public:
	MapInteractionService(Renderer& renderer, Camera& camera, const InputManager& inputManager,
	                      EntityRegistry& entityRegistry);
	~MapInteractionService() = default;

	void Tick() const;

private:
	void _HandleMouseMove(LLGL::Offset2D mousePos);
	void _OnClick();

	glm::vec3 _CalculateMouseRay(glm::vec2 mousePos) const;
	bool _Intersect(glm::vec3 position, glm::vec3 size, glm::vec3 intersection);
	float _WithinMapBounds(glm::vec3 position, glm::vec3 size, glm::vec3 mouseRay) const;

	glm::vec2 mMousePos = {0, 0};
	glm::vec3 mIntersectionPoint = {0, 0, 0};
	std::shared_ptr<DrawData> mCurrentSelectedObject = nullptr;

	Camera& mCamera;
	Renderer& mRenderer;
	EntityRegistry& mEntityRegistry;

	std::shared_ptr<DrawData> mDebugBox;

	std::vector<Vertex> vertices = {
		{{-0.5, -0.5, 1}, {1, 1, 1}, {0, 0}}, // top left
		{{-0.5, 0.5, 1}, {1, 1, 1}, {0, 1}}, // bottom left
		{{0.5, -0.5, 1}, {1, 1, 1}, {1, 0}}, // top right
		{{0.5, 0.5, 1}, {1, 1, 1}, {1, 1}}, // bottom right
	};
};
