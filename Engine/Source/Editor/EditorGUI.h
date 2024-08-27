#pragma once
#include "MapEditor.h"
#include "ObjectGUI.h"

class EntityRegistry;
class ResourceManager;

class EditorGUI
{
public:
	EditorGUI();

	void Render(EntityRegistry& entityRegistry, ResourceManager& resourceManager) const;

private:
	void _MainMenu(Camera& camera) const;

	std::unique_ptr<MapEditor> mMapEditor;
	std::unique_ptr<ObjectGUI> mObjectGUI;
	//InputHandler& mInputHandler;
};
