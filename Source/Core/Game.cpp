#include "DescriptionRegistry.h"
#include "GUISystem.h"
#include "InputHandler.h"
#include "TileSetEditor.h"
#include "Core/RendererInstance.h"
#include "Core/Window.h"
#include "Camera.h"

#include "Game.h"

int main(int argc, char** argv)
{
	auto app = std::make_unique<Game>();
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
	//mDescriptionRegistry = std::make_unique<DescriptionRegistry>();
	//mDescriptionRegistry->registerAllDescriptions();
	mRenderer = RendererInstance::GetInstance();
	mWindow = std::make_unique<Window>();

	mRenderer->OnDrawInit();
	
	mCamera = std::make_unique<Camera>();
	mCamera->UpdateView();

	mInputHandler = std::make_unique<InputHandler>(*mWindow);
	mInputHandler->registerButtonUpHandler(LLGL::Key::Escape, [=]() { mRunning = false; });

	// Register camera handlers
	mInputHandler->registerButtonUpHandler(LLGL::Key::W, [=]() { mCamera->MoveUp(); });
	mInputHandler->registerButtonUpHandler(LLGL::Key::S, [=]() { mCamera->MoveDown(); });
	mInputHandler->registerButtonUpHandler(LLGL::Key::A, [=]() { mCamera->MoveLeft(); });
	mInputHandler->registerButtonUpHandler(LLGL::Key::D, [=]() { mCamera->MoveRight(); });
	
	mGUISystem = std::make_unique<GUISystem>();
	mTileSetEditor = std::make_unique<TileSetEditor>();
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
			mGUISystem->GUIEndFrame();
		});
		
	}
}
