#pragma once
#include <string>

struct SpriteComponent
{
	std::string mTexturePath;
	int mRows = 0;
	int mColumns = 0;

	// TODO: Needs to be set by ResourceManager
	// Runtime data for texture id - rendering
	int mTextureId = 0;
};
