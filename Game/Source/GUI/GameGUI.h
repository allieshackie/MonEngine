#pragma once
#include "GUI/GUIBase.h"

class EngineContext;
class LevelManager;

class GameGUI : public GUIBase
{
public:
	GameGUI(EngineContext& engineContext);
	~GameGUI() override = default;

	void RenderGUI() override;

private:
	void _MainMenu();
	void _SelectLevelMenu(bool* p_open);

	void _LevelMenu();

	void _GetAllLevelFileNames();

	bool show_main_menu = true;
	bool show_select_level_menu = false;

	int current_level_selected = 0;

	std::vector<char*> levelFileNames;

	EngineContext& mEngineContext;

	const char* EDITOR_LEVEL = "editor";
};
