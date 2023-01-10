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
	mGUISystem = std::make_unique<GUISystem>(*mRenderer);

	mInputHandler = std::make_shared<InputHandler>(*mGUISystem);
	mInputManager = std::make_unique<InputManager>(*mInputHandler);

	auto& llglWindow = mWindow->GetWindow();
	llglWindow.AddEventListener(mInputHandler);

	mCamera = std::make_unique<Camera>(*mRenderer, *mInputManager);

	// Register exit button
	mInputManager->registerButtonUpHandler(LLGL::Key::Escape, [=]() { mRunning = false; });

	mInteractionManager = std::make_unique<InteractionManager>(*mRenderer, *mCamera, *mInputManager, *mResourceManager);

	// Editors
	mTileSetEditor = std::make_unique<TileSetEditor>(*mRenderer, *mResourceManager, *mCamera);
	mMapEditor = std::make_unique<MapEditor>(*mRenderer, *mResourceManager, *mInteractionManager);
}

void Game::closeGame()
{
	mRunning = false;
}

void Game::runGame() const
{
	//mResourceManager->AddTile("tiles.jpg", {0, 0, 10}, {1, 1, 1}, {.1, .1}, {1, 1});
	//mResourceManager->CreateBox(*mRenderer, {0, 0, 10}, {1, 1, 1}, {255, 0, 0});
	mResourceManager->AddSprite("awesomeface.png", {0, 0, 10}, {1, 1, 1});
	//mResourceManager->CreateMap({0, 0, 10}, "map0");
	while (mRenderer->GetSwapChain().GetSurface().ProcessEvents() && mRunning)
	{
		mRenderer->OnDrawFrame([=]()
		{
			// Render GUI
			mGUISystem->GUIStartFrame();
			//mTileSetEditor->RenderGUI();
			//mMapEditor->RenderGUI();
			mTileSetEditor->RenderTest();
			mGUISystem->GUIEndFrame();
		});
	}
}
