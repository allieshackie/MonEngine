#pragma once
#include <glm/vec3.hpp>
#include "Core/Vertex.h"
#include "LLGL/Types.h"

class Camera;
class DrawData;
class InputManager;
class MapSystem;
class Renderer;

class MapInteractionSystem
{
public:
	MapInteractionSystem(MapSystem& mapSystem, Renderer& renderer, Camera& camera,
	                     const InputManager& inputManager);
	~MapInteractionSystem() = default;

	void SetPaletteBrush(int brushIndex);
	void Tick();

private:
	void _HandleMouseMove(LLGL::Offset2D mousePos);
	void _OnClick();

	glm::vec3 _CalculateMouseRay(glm::vec2 mousePos) const;
	bool _Intersect(glm::vec3 position, glm::vec3 size, glm::vec3 intersection);
	float _WithinMapBounds(glm::vec3 position, glm::vec3 size, glm::vec3 mouseRay) const;

	glm::vec2 mMousePos = {0, 0};
	glm::vec3 mIntersectionPoint = {0, 0, 0};
	glm::vec3 mMapTopLeft = {0, 0, 0};

	Camera& mCamera;
	Renderer& mRenderer;
	MapSystem& mMapSystem;

	std::shared_ptr<DrawData> mDebugBox;

	int mCurrentBrushIndex = -1;

	std::vector<Vertex> vertices = {
		{{-0.5, -0.5, 1}, {1, 1, 1}, {0, 0}}, // top left
		{{-0.5, 0.5, 1}, {1, 1, 1}, {0, 1}}, // bottom left
		{{0.5, -0.5, 1}, {1, 1, 1}, {1, 0}}, // top right
		{{0.5, 0.5, 1}, {1, 1, 1}, {1, 1}}, // bottom right
	};
};
