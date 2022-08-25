#include "DescriptionRegistry.h"
#include "GUISystem.h"
#include "InputHandler.h"
#include "TileSetEditor.h"
#include "Core/RendererInstance.h"
#include "Core/Window.h"
#include "Camera.h"
#include "Core/ResourceManager.h"

#include "Game.h"

#include "Map.h"


int main(int argc, char** argv)
{
	const auto app = std::make_unique<Game>();
	app->configureLevel();
	app->runGame();
	
	return 0;
}

Window& Game::getWindow() const
{
    return *mWindow;
}

void Game::configureLevel()
{
	mRenderer = RendererInstance::GetInstance();
	mWindow = std::make_unique<Window>();

	mCamera = std::make_unique<Camera>();
	mCamera->UpdateView();

	mInputHandler = std::make_unique<InputHandler>(*mWindow);
	mInputHandler->registerButtonUpHandler(LLGL::Key::Escape, [=]() { mRunning = false; });

	// Register camera handlers
	mInputHandler->registerButtonUpHandler(LLGL::Key::Up, [=]() { mCamera->MoveUp(); });
	mInputHandler->registerButtonUpHandler(LLGL::Key::Down, [=]() { mCamera->MoveDown(); });
	mInputHandler->registerButtonUpHandler(LLGL::Key::Left, [=]() { mCamera->MoveLeft(); });
	mInputHandler->registerButtonUpHandler(LLGL::Key::Right, [=]() { mCamera->MoveRight(); });

	mInputHandler->registerZoomInHandler([=]() { mCamera->ZoomIn(); });
	mInputHandler->registerZoomOutHandler([=]() { mCamera->ZoomOut(); });

	mGUISystem = std::make_unique<GUISystem>();
	mTileSetEditor = std::make_unique<TileSetEditor>(*mCamera);
}

void Game::closeGame()
{
	mRunning = false;
}

void Game::runGame() const
{
	Map* map = new Map();
	map->LoadMap("map1");
	while (mRenderer->GetContext().GetSurface().ProcessEvents() && mRunning)
	{
		// Process Input
		if (mGUISystem->isGUIContext())
		{
			mInputHandler->pollGUIInputEvents([=](const InputEvent& event)
			{
				mGUISystem->handleGUIInput(event);
			});
		}
		else
		{
			mInputHandler->pollInputEvents();	
		}
		mRenderer->OnDrawFrame([=]()
		{
			// Render GUI
			mGUISystem->GUIStartFrame();
			mTileSetEditor->RenderGUI();
			mGUISystem->GUIEndFrame();
		});
		
	}
}
