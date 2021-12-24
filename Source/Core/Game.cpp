#include <SDL.h>
#include <glad/glad.h>
#include "DescriptionRegistry.h"
#include "EntityRegistry.h"
#include "InputHandler.h"
#include "GUISystem.h"
#include "Map.h"
#include "Renderer.h"
#include "Window.h"

#include "Game.h"

int main(int argc, char** argv)
{
	auto app = std::make_unique<Game>();
	app->configureLevel();
	app->runGame();
	
	return 0;
}

Game::Game()
{
    mWindow = std::make_unique<Window>();
    mRenderer = std::make_unique<Renderer>(*mWindow);

	//mRenderer->initExample();
	
    mInputHandler = std::make_unique<InputHandler>();
    mInputHandler->registerEventHandler(SDL_QUIT, [=]() { mRunning = false; });


	mWindow->LLGLExample();

	//texture = new Texture(mWindow->getSDLWindow(), "ground_tileset_01.png");

	//mGUISystem = std::make_unique<GUISystem>(mWindow->getSDLWindow(), mWindow->getSDLContext());
}

Window& Game::getWindow() const
{
    return *mWindow;
}

void Game::configureLevel()
{
	mDescriptionRegistry = std::make_unique<DescriptionRegistry>();
	//mMap = std::make_unique<Map>(*mDescriptionRegistry, mRenderer->getRenderer());
}

void Game::closeGame()
{
	mRunning = false;
}

void Game::runGame() const
{
	while (mRunning) {
        
		mInputHandler->tick();

		//mGUISystem->renderGUI();

		//mRenderer->renderExample();

		//texture->Render(0, 0);

		//SDL_GL_SwapWindow(&mWindow->getSDLWindow());
		/*
		 * 
        mRenderer->render([=]() {
            //mMap->Render();
        });
		 */
	}
}
