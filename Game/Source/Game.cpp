#include "Core/Defines.h"
#include "Core/EventListener.h"
#include "Core/LevelManager.h"
#include "Core/Timer.h"
#include "Entity/EntityRegistry.h"
#include "Entity/Systems/CollisionSystem.h"
#include "Entity/Systems/PhysicsSystem.h"
#include "Entity/Systems/PlayerSystem.h"
#include "Graphics/Core/Window.h"
#include "Graphics/Core/ResourceManager.h"
#include "Graphics/Debug/DebugDraw.h"
#include "GUI/GameGUI.h"
#include "GUI/GUISystem.h"
#include "Input/InputHandler.h"
#include "Input/InputManager.h"
#include "Input/UIInputManager.h"
#include "Map/MapSystem.h"

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
	mEventPublisher = std::make_unique<EventPublisher>();
	mEntityRegistry = std::make_unique<EntityRegistry>(*mEventPublisher);
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
	mCollisionSystem = std::make_unique<CollisionSystem>(*mEntityRegistry, *mEventPublisher);
	mPhysicsSystem = std::make_unique<PhysicsSystem>(*mEntityRegistry);
	mPlayerSystem = std::make_unique<PlayerSystem>(*mEntityRegistry, *mInputManager);

	mGameGUI = std::make_unique<GameGUI>(*mLevelManager, *mPlayerSystem);

	mRenderer->InitPipelines(*mLevelManager, *mMapSystem);
	mRenderer->InitGUIPipeline(*mGUISystem, *mGameGUI);
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
