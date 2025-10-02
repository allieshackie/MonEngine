#pragma once
#include "MapEditor.h"
#include "Core/ISystem.h"
#include "ContextMenus/EntityMenu.h"
#include "ObjectGUI.h"

class Camera;
class InputHandler;
class RenderContext;
class ResourceManager;
class World;

class EditorGUI : public ISystem
{
public:
	EditorGUI(std::weak_ptr<InputHandler> inputHandler, std::weak_ptr<World> world, RenderContext& renderContext,
	          ResourceManager& resourceManager);

	void RenderGUI() override;

private:
	void _MainMenu(Camera& camera) const;

	std::unique_ptr<MapEditor> mMapEditor;
	std::unique_ptr<ObjectGUI> mObjectGUI;
	std::unique_ptr<EntityMenu> mEntityMenu;

	bool mOpen = true;
	ImVec2 mSize = {250, 300};
	ImGuiWindowFlags mWindowFlags = ImGuiWindowFlags_None;

	std::weak_ptr<InputHandler> mInputHandler;
	std::weak_ptr<World> mWorld;
};
