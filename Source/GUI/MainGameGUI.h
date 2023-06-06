#pragma once
#include "MapEditor.h"

class InputManager;
class LevelManager;
class MapSystem;
class Renderer;
class ResourceManager;

class MainGameGUI
{
public:
	MainGameGUI(InputManager& inputManager, LevelManager& levelManager, MapSystem& mapSystem, Renderer& renderer,
	            ResourceManager& resourceManager);
	~MainGameGUI() = default;

	void RenderGUI();

private:
	void _LevelMenu(bool* p_open);
	void _GetAllLevelFileNames();

	bool show_level_menu = false;
	int current_level_selected = 0;

	std::vector<char*> levelFileNames;

	std::unique_ptr<MapEditor> mMapEditor;

	const char* LEVELS_FOLDER = "../Data/Levels/";
	const char* EDITOR_LEVEL = "editor";
};
