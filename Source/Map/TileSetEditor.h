#pragma once

static const char* TEXTURE_FOLDER_GUI = "C:/dev/MonDev/Data/Textures";
static const char* TILESET_FOLDER = "C:/dev/MonDev/Data/Tileset";

class TileSetEditor {
public:
	TileSetEditor() = default;
	~TileSetEditor() = default;

	void RenderGUI();
private:
	void _NewTileSet(bool* p_open);
	void _LoadTextureMenu(bool* p_open);
	void _CurrentSpriteMenu(bool* p_open);
	
	void _CreateNewTileSetFile(char* fileNameBuf, char* textureFileName);

	void _LoadTileSetTexture(const char* textureName);
	
	void _GetTextureFileNames(std::array<char*, 6>& items);


	std::array<char*, 6> mTextureFileNames;
};