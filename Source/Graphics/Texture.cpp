#include "pch.h"

#include "Texture.h"

static std::string TEXTURE_PATH = "C:/dev/MonDev/Data/Textures/";

Texture::Texture(SDL_Renderer& ref, const std::string& path) : mRenderer(ref) {
	LoadFromFile(path);
}

Texture::~Texture() {
	Free();
}

bool Texture::LoadFromFile(const std::string& path) {
	// get rid of pre-existing texture?
	//Free();

	const std::string fullPath = TEXTURE_PATH.append(path);

	SDL_Texture* newTexture = NULL;

	// load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(fullPath.c_str());
	if (loadedSurface == NULL) {
		printf("Unable to load image %s! SDL_image Error: %s\n", fullPath.c_str(), IMG_GetError());
		return false;
	}

	SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
	newTexture = SDL_CreateTextureFromSurface(&mRenderer, loadedSurface);
	if (newTexture == NULL) {
		return false;
	}

	mWidth = loadedSurface->w;
	mHeight = loadedSurface->h;

	// get rid of old loaded surface
	SDL_FreeSurface(loadedSurface);

	mTexture = newTexture;
	return true;

}

void Texture::Free() {
	if (mTexture != NULL) {
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void Texture::Render(int x, int y, SDL_Rect* clip) {
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopy(&mRenderer, mTexture, clip, &renderQuad);
}

int Texture::GetWidth() const {
	return mWidth;
}

int Texture::GetHeight() const {
	return mHeight;
}