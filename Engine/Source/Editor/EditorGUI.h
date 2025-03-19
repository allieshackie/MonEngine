#pragma once
#include "MapEditor.h"
#include "ContextMenus/EntityMenu.h"
#include "ObjectGUI.h"

class Camera;
class InputHandler;
class MonScene;
class RenderContext;
class ResourceManager;
class SceneManager;

class EditorGUI
{
public:
	EditorGUI(const SceneManager& sceneManager, InputHandler& inputHandler);

	void Render(MonScene* scene, ResourceManager& resourceManager, const RenderContext& renderContext,
	            const InputHandler& inputHandler);

private:
	void _MainMenu(Camera& camera) const;

	std::unique_ptr<MapEditor> mMapEditor;
	std::unique_ptr<ObjectGUI> mObjectGUI;
	std::unique_ptr<EntityMenu> mEntityMenu;

	bool mOpen = true;
	ImVec2 mSize = {250, 300};
	ImGuiWindowFlags mWindowFlags = ImGuiWindowFlags_None;
	//InputHandler& mInputHandler;
};
