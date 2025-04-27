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

	void OnEntityAdded(Entity* entity);
	void OnEntityRemoved(Entity* entity);

	void QueueClick();

private:
	void _OnClick(const RenderContext& renderContext, const Camera& camera, PhysicsSystem& physicsSystem);
	void _HandleMouseMove(LLGL::Offset2D mousePos);

	glm::vec3 _CalculateMouseRay(const RenderContext& renderContext, const Camera& camera) const;
	bool _Intersect(glm::vec3 position, glm::vec3 size, glm::vec3 intersection);

	glm::vec2 mMousePos = {0, 0};
	std::vector<entt::entity> mEntityList;

	bool mQueuedClick = false;
};
