#pragma once

#include <SDL.h>
#include <iostream>


class Texture {
public:
	Texture(SDL_Window& ref, void* pixels, int width, int height, int stride);
	Texture(SDL_Window& ref, const std::string& path);
	~Texture();

	void Free();

	void Render(int x, int y) const;

	int GetWidth() const;

	int GetHeight() const;

private:
	bool LoadFromFile(const std::string& path);
	bool LoadFromPixels(void* pixels, int width, int height, int stride);

	
	SDL_Window& mWindow;
	SDL_Surface* mTexture = nullptr;

	unsigned int mTextureID = 0;
	unsigned int texture1 = 0;
	unsigned int texture2 = 0;
	unsigned int VAO = 0;
	unsigned int VBO = 0;
	unsigned int EBO = 0;

	int mWidth = 0;
	int mHeight = 0;
};