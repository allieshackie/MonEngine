#pragma once
#include "MapEditor.h"
#include "Systems/PlayerSystem.h"

class InputManager;
class LevelManager;
class MapSystem;
class Renderer;
class ResourceManager;

class MainGameGUI
{
public:
	MainGameGUI(InputManager& inputManager, LevelManager& levelManager, MapSystem& mapSystem, Renderer& renderer,
	            ResourceManager& resourceManager, PlayerSystem& playerSystem);
	~MainGameGUI() = default;

	void RenderGUI();

private:
	void _MainMenu();
	void _SelectLevelMenu(bool* p_open);

	void _LevelMenu();
	void _PlayerInfo(bool* p_open) const;

	void _GetAllLevelFileNames();

	bool show_main_menu = true;
	bool show_select_level_menu = false;
	bool show_player_info_menu = false;

	int current_level_selected = 0;

	std::vector<char*> levelFileNames;

	std::unique_ptr<MapEditor> mMapEditor;

	LevelManager& mLevelManager;
	PlayerSystem& mPlayerSystem;

	const char* LEVELS_FOLDER = "../Data/Levels/";
	const char* EDITOR_LEVEL = "editor";
};
