#include "Game.h"
#include "EntityRegistry.h"
#include "EntityContext.h"
#include "Window.h"
#include "TileSetDescription.h"
#include "MapDescription.h"

int main(int argc, char** argv)
{
	auto app = new Game();
	app->registerDescriptions();
	app->configureLevel();
	app->runGame();
	delete app;
	
	return 0;
}

Game::Game()
{
    mWindow = std::make_unique<Window>();
    mRenderer = &(mWindow->getRenderer());
}

Game::~Game()
{
}

Window& Game::getWindow()
{
    return *mWindow;
}

void Game::configureLevel()
{
	mMap = std::make_unique<Map>(*mDescriptionRegistry, mWindow->getRenderer());
}

void Game::registerDescriptions()
{
	mDescriptionRegistry = std::make_unique<DescriptionRegistry>();
	mDescriptionRegistry->registerDescription<TileSetDescription>(TileSetDescription::JsonName);
	mDescriptionRegistry->registerDescription<MapDescription>(MapDescription::JsonName);
}

void Game::registerEntityTemplates()
{
}

void Game::closeGame()
{
	mRunning = false;
}

void Game::runGame()
{
	while (mRunning) {
        /*
        //Handle events on queue
        while (SDL_PollEvent(&e) != 0)
        {
            //User requests quit
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
        }
        */

        //Clear screen
        SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(mRenderer);

        //Render texture to screen
		mMap->Render();

        //Update screen
        SDL_RenderPresent(mRenderer);
	}
}
