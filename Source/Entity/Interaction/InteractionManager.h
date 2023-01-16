#pragma once
#include "LLGL/Types.h"
#include <glm/vec3.hpp>

class Camera;
class InputManager;
class Renderer;
class RenderObject;
class ResourceManager;

class InteractionManager
{
public:
	InteractionManager(Renderer& renderer, Camera& camera, const InputManager& inputManager,
	                   ResourceManager& resourceManager);
	~InteractionManager() = default;

	void RegisterAction(std::function<void (RenderObject&)> cb);
	void AddInteractableObject(const std::shared_ptr<RenderObject>& obj);

private:
	void _HandleMouseMove(LLGL::Offset2D mousePos);
	void _OnClick() const;
	bool _Intersects(const RenderObject& obj, glm::vec3 mouseRay);

	glm::vec3 _CalculateMouseRay(LLGL::Offset2D mousePos) const;
	glm::vec3 _GetPlaneNormal(const RenderObject& obj) const;
	bool _IntersectTest(const RenderObject& obj, glm::vec3 mouseRay);

	std::vector<std::shared_ptr<RenderObject>> mInteractableObjects;
	glm::vec3 mMousePos = {0, 0, 0};
	std::vector<std::function<void(RenderObject&)>> mActionCallbacks;
	std::shared_ptr<RenderObject> mCurrentSelectedObject = nullptr;

	Camera& mCamera;
	Renderer& mRenderer;
	ResourceManager& mResourceManager;
};
