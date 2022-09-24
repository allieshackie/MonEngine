#pragma once

class Camera;
class Map;
class Sprite;
class Texture;
class Tile;
class TileSetDescription;

static const char* TEXTURE_FOLDER_GUI = "../Data/Textures";
static const char* TILESET_FOLDER = "../Data/Tileset";
static const char* MAP_FOLDER = "../Data/Maps";
static const char* TEXT_FILE = ".txt";

typedef unsigned int GLuint;

class TileSetEditor {
public:
	TileSetEditor(Camera& camera);
	~TileSetEditor() = default;

	void RenderGUI();
	void RenderTest();

private:
	// File display helpers
	void _GetTilesetFileNames();
	void _GetTextureFileNames();
	void _GetAllMapFileNames();

	// TILESET
	void _NewTileSet(bool* p_open);
	void _CreateNewTileSetFile(char* fileNameBuf, char* textureFileName);
	void _LoadTileSetMenu(bool* p_open);
	void _TileSetInfoMenu(bool *p_open);
	void _LoadTextureMenu(bool* p_open);
	void _LoadTileSetTexture(const char* textureName, bool debugGrid);
	void _TextureDisplayMenu(bool* p_open);

	// MAP
	void _NewMapMenu(bool* p_open);
	void _LoadMapMenu(bool* p_open);

	// PALLETTE
	void _PalletteMenu(bool* p_open);

	// DEBUG
	void _CreateMapDebugGrid(const Map& map);
	void _CreateTextureDebugGrid() const;

	void _CameraInfo(bool* p_open);

	void _CenterWindow(float width, float height);

	Tile* mCurrentSprite = nullptr;
	Camera& mCamera;
	TileSetDescription* mCurrentTileset = nullptr;
	ImGuiWindowFlags mWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

	Map* mCurrentMap = nullptr;

	GLuint mPalletteTextureId = -1;
	int mPalletteTextureWidth = 0;
	int mPalletteTextureHeight = 0;

	GLuint mBrushTextureId = -1;
	int mBrushTextureWidth = 0;
	int mBrushTextureHeight = 0;

	bool show_new_tileset_menu = false;
	bool show_load_tileset_menu = false;
	bool show_tileset_info_menu = false;

	bool show_new_map_menu = false;
	bool show_load_map_menu = false;

	bool show_load_texture_menu = false;
	bool show_texture_display_data = false;

	bool show_pallette_menu = false;

	bool show_camera_info = false;

	int current_texture_selected = 0;
	int current_tileset_selected = 0;
	int current_map_config_selected = 0;
	int current_map_selected = 0;
	int current_brush_index = 0;

	std::vector<char*> textureFileNames;
	std::vector<char*> tilesetFileNames;
	std::vector<char*> mapConfigFileNames;
	std::vector<char*> mapFileNames;
};