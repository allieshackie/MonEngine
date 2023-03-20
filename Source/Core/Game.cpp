#include "Core/Window.h"
#include "Core/ResourceManager.h"
#include "Camera.h"
#include "EntityTemplateRegistry.h"
#include "EntityRegistry.h"
#include "GUISystem.h"
#include "InputHandler.h"
#include "InputManager.h"
#include "Services/MapInteractionService.h"
#include "MapEditor.h"
#include "TileSetEditor.h"
#include "UIInputManager.h"
#include "Debug/DebugDraw.h"

#include "Game.h"

int main(int argc, char** argv)
{
	const auto app = std::make_unique<Game>();
	app->ConfigureLevel();
	app->RunGame();

	return 0;
}

Window& Game::GetWindow() const
{
	return *mWindow;
}

void Game::ConfigureLevel()
{
	mEntityTemplateRegistry = std::make_unique<EntityTemplateRegistry>();
	mEntityRegistry = std::make_unique<EntityRegistry>(*mEntityTemplateRegistry);

	mResourceManager = std::make_unique<ResourceManager>();
	mRenderer = std::make_unique<Renderer>(*mResourceManager, *mEntityRegistry);
	mWindow = std::make_unique<Window>(*mRenderer);
	mGUISystem = std::make_unique<GUISystem>(*mRenderer);

	mInputHandler = std::make_shared<InputHandler>(*mGUISystem);
	mInputManager = std::make_unique<InputManager>(*mInputHandler);

	auto& llglWindow = mWindow->GetWindow();
	llglWindow.AddEventListener(mInputHandler);

	mCamera = std::make_unique<Camera>(*mRenderer, *mInputManager);

	// Register exit button
	mInputManager->registerButtonUpHandler(LLGL::Key::Escape, [=]() { mRunning = false; });
	mMapInteractionService = std::make_unique<MapInteractionService>(*mRenderer, *mCamera, *mInputManager,
	                                                                 *mEntityRegistry);

	// Editors
	//mTileSetEditor = std::make_unique<TileSetEditor>(*mRenderer, *mResourceManager, *mCamera);
	mMapEditor = std::make_unique<MapEditor>(*mRenderer, *mResourceManager, *mMapInteractionService);
}

void Game::CloseGame()
{
	mRunning = false;
}

void Game::RunGame() const
{
	// Debug calls need to happen on tick
	mEntityRegistry->CreateEntityFromTemplate("tiles_map");
	//mEntityRegistry->CreateEntityFromTemplate("test");
	while (mRenderer->GetSwapChain().GetSurface().ProcessEvents() && mRunning)
	{
		mMapInteractionService->Tick();
		// Render XYZ Axis
		/*
		 *
		DebugDrawManager::GetInstance()->DrawLine({-1, 0, 0}, {1, 0, 0}, {255, 0, 0});
		DebugDrawManager::GetInstance()->DrawBox({1, 0, 0}, 0.1f, {255, 0, 0});
		DebugDrawManager::GetInstance()->DrawLine({0, -1, 0}, {0, 1, 0}, {0, 255, 0});
		DebugDrawManager::GetInstance()->DrawBox({0, 1, 0}, 0.1f, {0, 255, 0});
		DebugDrawManager::GetInstance()->DrawLine({0, 0, -1}, {0, 0, 1}, {0, 0, 255});
		DebugDrawManager::GetInstance()->DrawBox({0, 0, 1}, 0.1f, {0, 0, 255});
		 */
		mRenderer->OnDrawFrame([=]()
		{
			// Render GUI
			mGUISystem->GUIStartFrame();
			//mTileSetEditor->RenderGUI();
			mMapEditor->RenderGUI();
			//mTileSetEditor->RenderTest();
			mGUISystem->GUIEndFrame();
		});
	}
}
