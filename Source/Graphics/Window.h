#pragma once

#include <SDL.h>

static constexpr int SCREEN_WIDTH = 800;
static constexpr int SCREEN_HEIGHT = 600;

struct Vertex
{
    float position[2];
    uint8_t color[4]; // rgba
};

// Vertex data (3 vertices for example triangle)
const float s = 0.5f;

class Window {
public:
    Window();
    ~Window();

    SDL_Window& getSDLWindow();
    SDL_GLContext& getSDLContext();
	
    void LLGLExample();
    
private:
    bool initSDL();

    void closeSDL();

private:
    //The window we'll be rendering to
    SDL_Window* mSDLWindow = nullptr;
    SDL_GLContext mContext = nullptr;

    
};