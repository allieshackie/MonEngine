#pragma once

class Camera;
class Sprite;
class Texture;
class DrawData;
class TileSetDescription;
class Renderer;
class ResourceManager;

using GLuint = unsigned int;

class TileSetEditor
{
public:
	TileSetEditor(Renderer& renderer, ResourceManager& resourceManager, Camera& camera);
	~TileSetEditor() = default;

	void RenderGUI();
	void RenderTest();

private:
	// File display helpers
	void _GetTilesetFileNames();
	void _GetTextureFileNames();

	// TILESET
	void _NewTileSet(bool* p_open);
	void _CreateNewTileSetFile(char* fileNameBuf, char* textureFileName);
	void _LoadTileSetMenu(bool* p_open);
	void _TileSetInfoMenu(bool* p_open);
	void _LoadTextureMenu(bool* p_open);
	void _LoadTileSetTexture(const char* textureName);
	void _TextureDisplayMenu(bool* p_open);

	void _CameraInfo(bool* p_open) const;

	void _CenterWindow(float width, float height);

	std::shared_ptr<DrawData> mCurrentSprite = nullptr;
	Camera& mCamera;
	std::shared_ptr<TileSetDescription> mCurrentTileset;
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

	const char* TEXTURE_FOLDER_GUI = "../Data/Textures";
	const char* TILESET_FOLDER = "../Data/Tileset";

	Renderer& mRenderer;
	ResourceManager& mResourceManager;
};
