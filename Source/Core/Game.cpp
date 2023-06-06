#include "Core/Window.h"
#include "Core/ResourceManager.h"
#include "Debug/DebugDraw.h"
#include "Entity/Systems/CollisionSystem.h"
#include "Entity/Systems/PhysicsSystem.h"
#include "Entity/Systems/PlayerSystem.h"
#include "Defines.h"
#include "EntityRegistry.h"
#include "InputHandler.h"
#include "InputManager.h"
#include "GUISystem.h"
#include "LevelManager.h"
#include "MainGameGUI.h"
#include "MapSystem.h"
#include "Timer.h"
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
	mTimer = std::make_unique<Timer>();

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

	// systems
	mCollisionSystem = std::make_unique<CollisionSystem>(*mEntityRegistry);
	mPhysicsSystem = std::make_unique<PhysicsSystem>(*mEntityRegistry);
	mPlayerSystem = std::make_unique<PlayerSystem>(*mEntityRegistry, *mInputManager);

	mMainGameGUI = std::make_unique<MainGameGUI>(*mInputManager, *mLevelManager, *mMapSystem,
	                                             *mRenderer, *mResourceManager, *mPlayerSystem);

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
static void _DrawAxis()
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

void Game::RunGame() const
{
	// Init current time
	mTimer->mCurrentTime = Clock::now();

	while (mRenderer->GetSwapChain().GetSurface().ProcessEvents() && mRunning)
	{
		auto frameTime = Clock::now();
		Duration elapsed = frameTime - mTimer->mCurrentTime;
		mTimer->mCurrentTime = frameTime;

		double deltaTime = elapsed.count();
		if (deltaTime > 0.25)
		{
			// Clamp large deltaTime to prevent "spiral of death"
			// where we could potentially always be "catching up"
			deltaTime = 0.25;
		}

		mTimer->mAccumulator += deltaTime;

		while (mTimer->mAccumulator >= deltaTime)
		{
			Update();
			mTimer->mAccumulator -= deltaTime;
		}

		// Debug draw
		_DrawAxis();
		mRenderer->Render();
	}
}

void Game::Update() const
{
	mPhysicsSystem->Update(mTimer->mDT);
}
