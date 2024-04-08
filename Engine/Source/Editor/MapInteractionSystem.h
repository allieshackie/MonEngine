#pragma once
#include <glm/vec3.hpp>
#include "LLGL/Types.h"
#include "Graphics/Core/Vertex.h"

class Camera;
class DrawData;
class InputHandler;
class Map;
class MapRegistry;
class RenderContext;

class MapInteractionSystem
{
public:
	MapInteractionSystem(Camera& camera, InputHandler& inputHandler, MapRegistry& mapRegistry,
	                     RenderContext& renderContext);

	void SetPaletteBrush(int brushIndex);

private:
	void _HandleMouseMove(LLGL::Offset2D mousePos);
	void _OnClick();

	glm::vec3 _CalculateMouseRay(glm::vec2 mousePos) const;
	bool _Intersect(glm::vec3 position, glm::vec3 size, glm::vec3 intersection);
	float _WithinMapBounds(glm::vec3 position, glm::vec3 size, glm::vec3 mouseRay) const;
	void _CalculateTileInteractionData(const Map& map, int columns, int tileIndex, glm::vec3& pos,
	                                   glm::vec3& size) const;

	glm::vec2 mMousePos = {0, 0};
	glm::vec3 mIntersectionPoint = {0, 0, 0};

	RenderContext& mRenderContext;
	MapRegistry& mMapRegistry;

	std::shared_ptr<DrawData> mDebugBox;
	Camera& mCamera;

	int mCurrentBrushIndex = -1;

	std::vector<Vertex> vertices = {
		{{-0.5, -0.5, 1}, {1, 1, 1}, {0, 0}}, // top left
		{{-0.5, 0.5, 1}, {1, 1, 1}, {0, 1}}, // bottom left
		{{0.5, -0.5, 1}, {1, 1, 1}, {1, 0}}, // top right
		{{0.5, 0.5, 1}, {1, 1, 1}, {1, 1}}, // bottom right
	};
};
