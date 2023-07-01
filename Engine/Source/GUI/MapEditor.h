#pragma once
#include "Map/MapInteractionSystem.h"

class Camera;
class InputManager;
class LevelManager;
class MapSystem;
class Map;
class Renderer;
class ResourceManager;

using GLuint = unsigned int;

class MapEditor
{
public:
	MapEditor(InputManager& inputManager, LevelManager& levelManager, MapSystem& mapSystem, Renderer& renderer,
	          ResourceManager& resourceManager);
	~MapEditor() = default;

	void InitCameraInputs() const;
	void RenderGUI();

private:
	// MAP
	void _NewMapMenu(bool* p_open);
	void _LoadMapMenu(bool* p_open);

	void _LoadMap(const char* mapName);
	void _MapMenu(bool* p_open) const;
	void _PaletteMenu(bool* p_open);

	void _PreviewTexture(const char* mapName);
	void _TextureMenu(bool* p_open) const;

	// File display helpers
	void _GetAllMapFileNames();
	void _CenterWindow(float width, float height);
	void _CameraInfo(bool* p_open) const;

	void _DrawTextureDebugGrid() const;
	void _DrawMapDebugGrid() const;

	bool show_palette_menu = false;
	bool show_map_menu = false;

	bool show_new_map_menu = false;
	bool show_load_map_menu = false;

	bool show_camera_info = false;

	bool draw_debug_texture_grid = false;
	bool draw_debug_map_grid = false;
	bool show_texture_menu = false;

	int current_map_selected = 0;
	int current_brush_index = 0;

	GLuint mPalletteTextureId = -1;
	int mPalletteTextureWidth = 0;
	int mPalletteTextureHeight = 0;

	ImGuiWindowFlags mWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

	std::vector<char*> mapFileNames;
	std::vector<char*> textureFileNames;

	const char* MAP_FOLDER = "../Data/Maps/";
	const char* TEXTURES_FOLDER = "../Data/Textures/";

	InputManager& mInputManager;
	MapSystem& mMapSystem;
	Renderer& mRenderer;
	ResourceManager& mResourceManager;

	std::shared_ptr<Camera> mEditorCamera;
	std::unique_ptr<MapInteractionSystem> mMapInteractionSystem;
};
