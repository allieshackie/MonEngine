#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "Renderer.h"

static constexpr int SCREEN_WIDTH = 640;
static constexpr int SCREEN_HEIGHT = 480;

class Window {
public:
    Window();
    ~Window();

    SDL_Renderer& getRenderer();

private:
    bool InitSDL();
    
    void CloseSDL();

private:
    //The window we'll be rendering to
    SDL_Window* mSDLWindow = nullptr;

    //The window renderer
    SDL_Renderer* mSDLRenderer = nullptr;
};