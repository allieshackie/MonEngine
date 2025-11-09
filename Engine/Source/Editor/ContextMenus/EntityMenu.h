#pragma once
#include <imgui.h>
#include "LLGL/Types.h"

class Camera;
class InputHandler;
class Entity;
class RenderContext;
class World;

class EntityMenu
{
public:
	EntityMenu(std::weak_ptr<InputHandler> inputHandler, std::weak_ptr<World> world, RenderContext& renderContext);

	void Render();

	void OnEntityAdded(Entity* entity);
	void OnEntityRemoved(Entity* entity);
	void RenderSelectedEntityMenu();
	void RenderEntitySelection();

	void QueueClick();

	entt::entity GetSelectedEntity() const { return mSelectedEntity; }

private:
	void _HandleMouseMove(LLGL::Offset2D mousePos);
	glm::vec3 _CalculateMouseRay(glm::vec2 mousePos, const RenderContext& renderContext, const Camera& camera) const;
	void _OnClick(const RenderContext& renderContext, const Camera& camera);

	glm::vec2 mMousePos = {0, 0};

	// Entity Selection
	entt::entity mSelectedEntity;
	std::vector<entt::entity> mEntityList;
	std::vector<const char*> items;
	int current_entity_selected = 0;

	bool mQueuedClick = false;

	std::weak_ptr<World> mWorld;
	RenderContext& mRenderContext;
};
