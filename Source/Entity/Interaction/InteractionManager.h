#pragma once
#include "LLGL/Types.h"
#include <glm/vec3.hpp>

class Camera;
class DrawData;
class InputManager;
class Renderer;
class ResourceManager;

class InteractionManager
{
public:
	InteractionManager(Renderer& renderer, Camera& camera, const InputManager& inputManager,
	                   ResourceManager& resourceManager);
	~InteractionManager() = default;

	void RegisterAction(std::function<void (DrawData&)> cb);
	void AddInteractableObject(const std::shared_ptr<DrawData>& obj);

private:
	void _HandleMouseMove(LLGL::Offset2D mousePos);
	void _OnClick() const;
	bool _Intersects(const DrawData& obj, glm::vec3 mouseRay);

	glm::vec3 _CalculateMouseRay(LLGL::Offset2D mousePos) const;
	glm::vec3 _GetPlaneNormal(const DrawData& obj) const;
	bool _IntersectTest(const DrawData& obj, glm::vec3 mouseRay);

	std::vector<std::shared_ptr<DrawData>> mInteractableObjects;
	glm::vec3 mMousePos = {0, 0, 0};
	std::vector<std::function<void(DrawData&)>> mActionCallbacks;
	std::shared_ptr<DrawData> mCurrentSelectedObject = nullptr;

	Camera& mCamera;
	Renderer& mRenderer;
	ResourceManager& mResourceManager;

	std::shared_ptr<DrawData> mDebugBox;

	std::vector<Vertex> vertices = {
		{{-0.5, -0.5, 1}, {1, 1, 1}, {0, 0}}, // top left
		{{-0.5, 0.5, 1}, {1, 1, 1}, {0, 1}}, // bottom left
		{{0.5, -0.5, 1}, {1, 1, 1}, {1, 0}}, // top right
		{{0.5, 0.5, 1}, {1, 1, 1}, {1, 1}}, // bottom right
	};
};
