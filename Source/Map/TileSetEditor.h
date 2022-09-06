#pragma once

class Camera;
class Sprite;
class Tile;
class TileSetDescription;

static const char* TEXTURE_FOLDER_GUI = "../Data/Textures";
static const char* TILESET_FOLDER = "../Data/Tileset";

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

	// New Tileset
	void _NewTileSet(bool* p_open);
	void _CreateNewTileSetFile(char* fileNameBuf, char* textureFileName);
	// Load Tileset
	void _LoadTileSetMenu(bool* p_open);
	void _TileSetInfoMenu(bool *p_open);
	// Display Tileset
	void _LoadTextureMenu(bool* p_open);
	void _LoadTileSetTexture(const char* textureName, bool debugGrid);
	void _TextureDisplayMenu(bool* p_open);

	void _CreateDebugGrid(int rows = -1, int columns = -1);

	void _CameraInfo(bool* p_open);

	Tile* mCurrentSprite = nullptr;
	Camera& mCamera;
	TileSetDescription* mCurrentTileset = nullptr;
	ImGuiWindowFlags mWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

	bool show_new_tileset_menu = false;
	bool show_load_tileset_menu = false;
	bool show_tileset_info_menu = false;
	bool show_load_texture_menu = false;
	bool show_texture_display_data = false;
	bool show_camera_info = false;
	int current_texture_selected = 0;
	int current_tileset_selected = 0;
	std::vector<char*> textureFileNames;
	std::vector<char*> tilesetFileNames;
};