#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "LLGL/Types.h"

class InputHandler;
class RenderContext;
class PhysicsSystem;
class SceneManager;

class InteractSystem
{
public:
	InteractSystem(InputHandler& inputHandler, const SceneManager& sceneManager);

	void Update(const RenderContext& renderContext, const Camera& camera, PhysicsSystem& physicsSystem);
	void Render();

	void OnEntityAdded(Entity* entity);
	void OnEntityRemoved(Entity* entity);

	void DragClick();
	void PressClick();

private:
	void _OnClick(const RenderContext& renderContext, const Camera& camera, PhysicsSystem& physicsSystem) const;
	void _HandleMouseMove(LLGL::Offset2D mousePos);
	glm::vec3 _CalculateMouseRay(const RenderContext& renderContext, const Camera& camera) const;

	void _ShowOptions();

	glm::vec2 mMousePos = {0, 0};
	std::vector<entt::entity> mEntityList;

	bool mQueueIntersectCheck = false;

	bool mShowOptions = false;
};
