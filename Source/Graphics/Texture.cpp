#include <glad/glad.h>

#include "Texture.h"

static std::string TEXTURE_PATH = "C:/dev/MonDev/Data/Textures/";

Texture::Texture(SDL_Window& ref, void* pixels, int width, int height, int stride) : mWindow(ref)
{
	LoadFromPixels(pixels, width, height, stride);
}

Texture::Texture(SDL_Window& ref, const std::string& path) : mWindow(ref) {
	LoadFromFile(path);
}

Texture::~Texture() {
	Free();
}

bool Texture::LoadFromFile(const std::string& path) {
	std::string fullPath = TEXTURE_PATH;
	fullPath.append(path);

	mTexture = SDL_LoadBMP(fullPath.c_str());

	//glEnable(GL_TEXTURE_2D);
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, mTexture->format->BytesPerPixel, mTexture->w, mTexture->h, 0, GL_RGB, GL_UNSIGNED_BYTE, mTexture->pixels);

	return true;

}

bool Texture::LoadFromPixels(void* pixels, int width, int height, int stride)
{
	/*
	 * 
	SDL_Surface* loadedSurface = SDL_CreateRGBSurfaceWithFormatFrom(pixels, width, height, 24, stride, SDL_PIXELFORMAT_BGRA8888);
	if (loadedSurface == nullptr) {
		printf("Surface could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
	SDL_Texture* newTexture = SDL_CreateTextureFromSurface(&mRenderer, loadedSurface);
	if (newTexture == nullptr) {
		return false;
	}

	mWidth = loadedSurface->w;
	mHeight = loadedSurface->h;

	// get rid of old loaded surface
	SDL_FreeSurface(loadedSurface);

	mTexture = newTexture;
	 */
	return true;
}

void Texture::Free() {
	if (mTexture != nullptr) {
		mTexture = nullptr;
		mWidth = 0;
		mHeight = 0;

		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}
}

void Texture::Render(int x, int y) const {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

	// TODO: Must activate and bind textures
    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    // render container
    //mShader->use();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

int Texture::GetWidth() const {
	return mWidth;
}

int Texture::GetHeight() const {
	return mHeight;
}