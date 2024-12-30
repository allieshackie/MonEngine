#pragma once
#include <imgui.h>

class InputHandler;
class Entity;
class MonScene;
class SceneManager;
class ResourceManager;

class EntityMenu
{
public:
	EntityMenu(const SceneManager& sceneManager, InputHandler& inputHandler);

	void RenderSelectedEntityMenu(MonScene* scene, const ResourceManager& resourceManager);
	void RenderEntitySelection();

	void OnEntityAdded(Entity* entity);
	void OnEntityRemoved(Entity* entity);

private:
	void _HandleMouseMove(LLGL::Offset2D mousePos);
	void _OnClick();

	bool mOpen = true;
	ImVec2 mSize = {250, 300};
	ImGuiWindowFlags mWindowFlags = ImGuiWindowFlags_None;
	glm::vec2 mMousePos = {0, 0};

	// Entity Selection
	Entity* mSelectedEntity = nullptr;
	std::vector<std::string> keys;
	std::vector<const char*> items;
};
