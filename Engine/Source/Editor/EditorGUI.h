#pragma once
#include "MapEditor.h"
#include "ObjectGUI.h"

class MonScene;
class ResourceManager;

class EditorGUI
{
public:
	EditorGUI();

	void Render(MonScene* scene, ResourceManager& resourceManager) const;

private:
	void _MainMenu(Camera& camera) const;

	std::unique_ptr<MapEditor> mMapEditor;
	std::unique_ptr<ObjectGUI> mObjectGUI;
	//InputHandler& mInputHandler;
};
