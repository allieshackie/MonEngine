#include "Core/Window.h"
#include "Core/ResourceManager.h"
#include "Debug/DebugDraw.h"
#include "Defines.h"
#include "EntityRegistry.h"
#include "InputHandler.h"
#include "InputManager.h"
#include "GUISystem.h"
#include "LevelManager.h"
#include "MainGameGUI.h"
#include "MapSystem.h"
#include "UIInputManager.h"

#include "Game.h"

int main(int argc, char** argv)
{
	const auto app = std::make_unique<Game>();
	app->ConfigureBaseGame();
	app->RunGame();

	return 0;
}

Window& Game::GetWindow() const
{
	return *mWindow;
}

void Game::ConfigureBaseGame()
{
	mEntityRegistry = std::make_unique<EntityRegistry>();
	mMapSystem = std::make_unique<MapSystem>();

	mResourceManager = std::make_unique<ResourceManager>();
	mRenderer = std::make_unique<Renderer>(*mEntityRegistry, *mResourceManager);
	mWindow = std::make_unique<Window>(*mRenderer);
	mGUISystem = std::make_unique<GUISystem>(*mRenderer);

	mInputHandler = std::make_shared<InputHandler>(*mGUISystem);
	mInputManager = std::make_unique<InputManager>(*mInputHandler);

	auto& llglWindow = mWindow->GetWindow();
	llglWindow.AddEventListener(mInputHandler);
	// Register exit button
	mInputManager->registerButtonUpHandler(LLGL::Key::Escape, [=]() { mRunning = false; });
	mLevelManager = std::make_unique<LevelManager>(*mMapSystem, *mInputManager);

	mMainGameGUI = std::make_unique<MainGameGUI>(*mInputManager, *mLevelManager, *mMapSystem, *mRenderer,
	                                             *mResourceManager);

	mRenderer->InitPipelines(*mLevelManager, *mMapSystem);
	mRenderer->InitGUIPipeline(*mGUISystem, *mMainGameGUI);
}

void Game::CloseGame()
{
	mRunning = false;
}

/*
 * Draw the XYZ axis at 0,0,0
 * Note: Not ideal for debugging since the axises are drawn in world
 * space and don't follow the camera
 * Ideally, this could be draw directly to screen space and remain in the view
 * like UI
 */
void Game::_DrawAxis()
{
	if (Defines::GetInstance()->GetDebugDraw())
	{
		DebugDrawManager::GetInstance()->DrawLine({-1, 0, 0}, {1, 0, 0}, {255, 0, 0});
		DebugDrawManager::GetInstance()->DrawBox({1, 0, 0}, {0.1f, 0.1f, 1.0f}, {255, 0, 0});
		DebugDrawManager::GetInstance()->DrawLine({0, -1, 0}, {0, 1, 0}, {0, 255, 0});
		DebugDrawManager::GetInstance()->DrawBox({0, 1, 0}, {0.1f, 0.1f, 1.0f}, {0, 255, 0});
		DebugDrawManager::GetInstance()->DrawLine({0, 0, -1}, {0, 0, 1}, {0, 0, 255});
		DebugDrawManager::GetInstance()->DrawBox({0, 0, 1}, {0.1f, 0.1f, 1.0f}, {0, 0, 255});
	}
}

void Game::RunGame()
{
	while (mRenderer->GetSwapChain().GetSurface().ProcessEvents() && mRunning)
	{
		_DrawAxis();
		mRenderer->OnDrawFrame();
	}
}
