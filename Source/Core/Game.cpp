#include "Core/Window.h"
#include "Camera.h"
#include "GUISystem.h"
#include "InputHandler.h"
#include "InputManager.h"
#include "Map.h"
#include "TileSetEditor.h"
#include "UIInputManager.h"

#include "Game.h"

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
	mRenderer = Renderer::GetInstance();
	mWindow = std::make_unique<Window>();

	mCamera = std::make_unique<Camera>();
	mCamera->UpdateView();

	mInputHandler = std::make_unique<InputHandler>(*mWindow);
	mInputManager = std::make_unique<InputManager>(*mInputHandler);
	mUIInputManager = std::make_unique<UIInputManager>(*mInputManager);
	mInputManager->registerButtonUpHandler(LLGL::Key::Escape, [=]() { mRunning = false; });

	// Register camera handlers
	mInputManager->registerButtonUpHandler(LLGL::Key::Up, [=]() { mCamera->MoveUp(); });
	mInputManager->registerButtonUpHandler(LLGL::Key::Down, [=]() { mCamera->MoveDown(); });
	mInputManager->registerButtonUpHandler(LLGL::Key::Left, [=]() { mCamera->MoveLeft(); });
	mInputManager->registerButtonUpHandler(LLGL::Key::Right, [=]() { mCamera->MoveRight(); });

	mInputManager->registerZoomInHandler([=]() { mCamera->ZoomIn(); });
	mInputManager->registerZoomOutHandler([=]() { mCamera->ZoomOut(); });

	mGUISystem = std::make_unique<GUISystem>();
	mTileSetEditor = std::make_unique<TileSetEditor>(*mCamera);
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
			//mTileSetEditor->RenderTest();
			mGUISystem->GUIEndFrame();
		});
		
	}
}
