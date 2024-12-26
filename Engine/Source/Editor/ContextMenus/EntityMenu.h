#pragma once
#include <imgui.h>
#include "Entity/EntityRegistry.h"

class InputHandler;
class ResourceManager;

class EntityMenu
{
public:
	EntityMenu(EntityRegistry& entityRegistry, InputHandler& inputHandler);

	void RenderSelectedEntityMenu(EntityRegistry& entityRegistry, const ResourceManager& resourceManager);
	void RenderEntitySelection();

	void OnEntityAdded(EnTTRegistry& registry, EntityId entity);
	void OnEntityRemoved(EnTTRegistry& registry, EntityId entity);

private:
	void _HandleMouseMove(LLGL::Offset2D mousePos);
	void _OnClick();

	bool mOpen = true;
	ImVec2 mSize = {250, 300};
	ImGuiWindowFlags mWindowFlags = ImGuiWindowFlags_None;
	glm::vec2 mMousePos = {0, 0};

	// Entity Selection
	EntityId mSelectedEntity = 0;
	std::vector<std::string> keys;
	std::vector<const char*> items;
};
