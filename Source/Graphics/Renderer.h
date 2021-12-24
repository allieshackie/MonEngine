#pragma once
#include <SDL.h>

typedef int GLint;
typedef unsigned int GLuint;

class Window;

/*
class RendererSDL {
	RendererSDL() {}

private: 
	SDL_Renderer* mSDLRenderer = nullptr;
};
*/

class Renderer {
public:
	Renderer(Window& window);
	~Renderer();

	SDL_Renderer& getRenderer() const;

	bool initExample();
	void renderExample() const;

	void render(const std::function<void()>& f) const;
	void destroy();

private:
	//The window renderer
	SDL_Renderer* mSDLRenderer = nullptr;


	GLuint mProgramID = 0;
	GLint mVertexPos2DLocation = -1;
	GLuint mVAO = 0;
	GLuint mVBO = 0;
	GLuint mEBO = 0;
	//Render flag
	bool mRenderQuad = true;
};