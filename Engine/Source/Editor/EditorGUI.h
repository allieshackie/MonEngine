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

	void Render(MonScene* scene, ResourceManager& resourceManager, const RenderContext& renderContext) const;

private:
	void _MainMenu(Camera& camera) const;

	std::unique_ptr<MapEditor> mMapEditor;
	std::unique_ptr<ObjectGUI> mObjectGUI;
	std::unique_ptr<EntityMenu> mEntityMenu;
	//InputHandler& mInputHandler;
};
