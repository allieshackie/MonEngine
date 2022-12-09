#include "Core/Window.h"
#include "Camera.h"
#include "GUISystem.h"
#include "InputHandler.h"
#include "InputManager.h"
#include "Interaction/InteractionManager.h"
#include "MapEditor.h"
#include "TileSetEditor.h"
#include "UIInputManager.h"

#include "Game.h"

#include "Core/ResourceManager.h"

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
	mResourceManager = std::make_unique<ResourceManager>();
	mRenderer = std::make_unique<Renderer>(*mResourceManager);
	mWindow = std::make_unique<Window>(*mRenderer);

	mCamera = std::make_unique<Camera>(*mRenderer);
	mCamera->UpdateView();

	mInputHandler = std::make_unique<InputHandler>(*mWindow);
	mInputManager = std::make_unique<InputManager>(*mInputHandler);

	mInputManager->registerButtonUpHandler(LLGL::Key::Escape, [=]() { mRunning = false; });

	// Register camera handlers
	mInputManager->registerButtonUpHandler(LLGL::Key::Up, [=]() { mCamera->MoveUp(); });
	mInputManager->registerButtonUpHandler(LLGL::Key::Down, [=]() { mCamera->MoveDown(); });
	mInputManager->registerButtonUpHandler(LLGL::Key::Left, [=]() { mCamera->MoveLeft(); });
	mInputManager->registerButtonUpHandler(LLGL::Key::Right, [=]() { mCamera->MoveRight(); });

	mInputManager->registerZoomInHandler([=]() { mCamera->ZoomIn(); });
	mInputManager->registerZoomOutHandler([=]() { mCamera->ZoomOut(); });

	mInteractionManager = std::make_unique<InteractionManager>(*mRenderer, *mCamera, *mInputManager, *mResourceManager);

	mGUISystem = std::make_unique<GUISystem>(*mRenderer);
	mTileSetEditor = std::make_unique<TileSetEditor>(*mRenderer, *mResourceManager, *mCamera);
	mMapEditor = std::make_unique<MapEditor>(*mRenderer, *mResourceManager, *mInteractionManager);
}

void Game::closeGame()
{
	mRunning = false;
}

void Game::runGame() const
{
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
			mMapEditor->RenderGUI();
			//mTileSetEditor->RenderTest();
			mGUISystem->GUIEndFrame();
		});
	}
}
