#pragma once

class InputManager;
class InteractionManager;
class Map;
class Renderer;
class ResourceManager;

using GLuint = unsigned int;

class MapEditor
{
public:
	MapEditor(Renderer& renderer, ResourceManager& resourceManager, InteractionManager& interactionManager);
	~MapEditor();

	void RenderGUI();

	static void ShowNewMapMenu();
	static void ShowLoadMapMenu();

	void PaintTile(DrawData& tile) const;

private:
	// PALLETTE
	void _PalletteMenu(bool* p_open);

	// MAP
	void _NewMapMenu(bool* p_open);
	void _LoadMapMenu(bool* p_open);

	void _LoadMap(const char* mapName);

	// File display helpers
	void _GetAllMapFileNames();

	void _CenterWindow(float width, float height);

	static bool show_pallette_menu;

	static bool show_new_map_menu;
	static bool show_load_map_menu;

	int current_map_selected = 0;
	int current_brush_index = 0;

	int current_hovered_tile_index = -1;
	std::shared_ptr<Map> mCurrentMap = nullptr;

	GLuint mPalletteTextureId = -1;
	int mPalletteTextureWidth = 0;
	int mPalletteTextureHeight = 0;

	ImGuiWindowFlags mWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

	std::vector<char*> mapFileNames;
	std::vector<char*> tilesetFileNames;

	const char* MAP_FOLDER = "../Data/Maps/";
	const char* TILESET_FOLDER = "../Data/Tileset";

	const char* TEXT_FILE = ".txt";

	Renderer& mRenderer;
	ResourceManager& mResourceManager;
	InteractionManager& mInteractionManager;
};
