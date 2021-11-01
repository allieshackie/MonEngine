#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

class Texture {
public:
	Texture(SDL_Renderer& ref, const std::string& path);
	~Texture();

	bool LoadFromFile(const std::string& path);

	void Free();

	void Render(int x, int y, SDL_Rect* clip = NULL);

	int GetWidth() const;

	int GetHeight() const;

private:
	SDL_Renderer& mRenderer;
	SDL_Texture* mTexture = nullptr;

	int mWidth = 0;
	int mHeight = 0;
};