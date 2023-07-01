#pragma once
#include "GUI/GUIBase.h"

class LevelManager;
class PlayerSystem;

class GameGUI : public GUIBase
{
public:
	GameGUI(LevelManager& levelManager, PlayerSystem& playerSystem);
	~GameGUI() override = default;

	void RenderGUI() override;

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

	LevelManager& mLevelManager;
	PlayerSystem& mPlayerSystem;

	const char* LEVELS_FOLDER = "../Data/Levels/";
	const char* EDITOR_LEVEL = "editor";
};
