#pragma once

class Camera;
class Sprite;
class Tile;

static const char* TEXTURE_FOLDER_GUI = "../Data/Textures";
static const char* TILESET_FOLDER = "../Data/Tileset";

class TileSetEditor {
public:
	TileSetEditor(Camera& camera);
	~TileSetEditor() = default;

	void RenderGUI();

	void RenderTest();
private:
	void _NewTileSet(bool* p_open);
	void _LoadTextureMenu(bool* p_open);
	void _CurrentSpriteMenu(bool* p_open);
	
	void _CreateNewTileSetFile(char* fileNameBuf, char* textureFileName);

	void _LoadTileSetTexture(const char* textureName);
	
	void _GetTextureFileNames(std::array<char*, 6>& items);

	void _CameraInfo(bool* p_open);


	std::array<char*, 6> mTextureFileNames;
	Tile* mCurrentSprite = nullptr;
	Camera& mCamera;
};