#pragma once
#include <imgui.h>
#include "LLGL/Types.h"

class Camera;
class InputHandler;
class Entity;
class MonScene;
class RenderContext;
class SceneManager;

class EntityMenu
{
public:
	EntityMenu(const SceneManager& sceneManager, InputHandler& inputHandler);

	void Render(MonScene* scene, const RenderContext& renderContext);

	void OnEntityAdded(Entity* entity);
	void OnEntityRemoved(Entity* entity);
	void RenderSelectedEntityMenu(MonScene* scene);
	void RenderEntitySelection(MonScene* scene);

	void QueueClick();

private:
	void _HandleMouseMove(LLGL::Offset2D mousePos);
	glm::vec3 _CalculateMouseRay(glm::vec2 mousePos, const RenderContext& renderContext, const Camera& camera) const;
	void _OnClick(const RenderContext& renderContext, const Camera& camera);

	bool mOpen = true;
	ImVec2 mSize = {250, 300};
	ImGuiWindowFlags mWindowFlags = ImGuiWindowFlags_None;
	glm::vec2 mMousePos = {0, 0};

	// Entity Selection
	Entity* mSelectedEntity = nullptr;
	std::vector<entt::entity> mEntityList;
	std::vector<const char*> items;
	int current_entity_selected = 0;

	bool mQueuedClick = false;
};
