#pragma once
#include "MapEditor.h"
#include "ContextMenus/EntityMenu.h"
#include "ObjectGUI.h"

class Camera;
class InputHandler;
class RenderContext;
class ResourceManager;
class World;

class EditorGUI
{
public:
	EditorGUI(InputHandler& inputHandler, std::weak_ptr<World> world, RenderContext& renderContext,
	          ResourceManager& resourceManager);

	void Render(float dt);

private:
	void _MainMenu(Camera& camera) const;

	std::unique_ptr<MapEditor> mMapEditor;
	std::unique_ptr<ObjectGUI> mObjectGUI;
	std::unique_ptr<EntityMenu> mEntityMenu;

	bool mOpen = true;
	ImVec2 mSize = {250, 300};
	ImGuiWindowFlags mWindowFlags = ImGuiWindowFlags_None;

	InputHandler& mInputHandler;
	std::weak_ptr<World> mWorld;
};
