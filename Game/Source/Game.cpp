#include "Core/ConfigManager.h"
#include "Core/Defines.h"
#include "Core/EventListener.h"
#include "Core/LevelManager.h"
#include "Core/Timer.h"
#include "Entity/EntityRegistry.h"
#include "Entity/Descriptions/DescriptionFactory.h"
#include "Entity/Descriptions/CollisionDescription.h"
#include "Entity/Descriptions/InteractiveDescription.h"
#include "Entity/Descriptions/PhysicsDescription.h"
#include "Entity/Descriptions/PlayerDescription.h"
#include "Entity/Descriptions/SpriteDescription.h"
#include "Entity/Descriptions/TransformDescription.h"
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
#include "Physics/CollisionSystem.h"
#include "Physics/PhysicsSystem.h"

#include "Game.h"

int main(int argc, char** argv)
{
	const auto app = std::make_unique<Game>();
	app->_ConfigureGame();
	app->RunGame();

	return 0;
}

void Game::_ConfigureGame()
{
	mConfigManager = std::make_unique<ConfigManager>();
	mConfigManager->LoadConfigFile("D:/dev/MonDev/Game/Data/", "config.json");

	mDescriptionFactory = std::make_unique<DescriptionFactory>();
	_RegisterDescriptions();

	mEventPublisher = std::make_unique<EventPublisher>();
	mEntityRegistry = std::make_unique<EntityRegistry>(*mDescriptionFactory, mConfigManager->GetEntitiesFolderPath(),
	                                                   *mEventPublisher);
	mMapSystem = std::make_unique<MapSystem>(mConfigManager->GetMapsFolderPath());
	mTimer = std::make_unique<Timer>();

	mResourceManager = std::make_unique<ResourceManager>(mConfigManager->GetTexturesFolderPath());
	mRenderer = std::make_unique<Renderer>(*mEntityRegistry, *mResourceManager, mConfigManager->GetShadersFolderPath());
	mWindow = std::make_unique<Window>(*mRenderer);
	mGUISystem = std::make_unique<GUISystem>(*mRenderer);

	mInputHandler = std::make_shared<InputHandler>(*mGUISystem);
	mInputManager = std::make_unique<InputManager>(*mInputHandler);

	auto& llglWindow = mWindow->GetWindow();
	llglWindow.AddEventListener(mInputHandler);
	// Register exit button
	mInputManager->registerButtonUpHandler(LLGL::Key::Escape, [=]() { mRunning = false; });
	mLevelManager = std::make_unique<LevelManager>(mConfigManager->GetLevelsFolderPath(), *mInputManager, *mMapSystem);

	// systems
	mCollisionSystem = std::make_unique<CollisionSystem>(*mEntityRegistry, *mEventPublisher);
	mPhysicsSystem = std::make_unique<PhysicsSystem>(*mEntityRegistry);
	mPlayerSystem = std::make_unique<PlayerSystem>(*mEntityRegistry, *mInputManager);

	mGameGUI = std::make_unique<GameGUI>(*mLevelManager, *mPlayerSystem, mConfigManager->GetLevelsFolderPath());

	mRenderer->InitPipelines(*mLevelManager, *mMapSystem);
	mRenderer->InitGUIPipeline(*mGUISystem, *mGameGUI);
}

void Game::_RegisterDescriptions() const
{
	// Engine Descriptions
	mDescriptionFactory->RegisterDescription<SpriteDescription>(SpriteDescription::JsonName);
	mDescriptionFactory->RegisterDescription<TransformDescription>(TransformDescription::JsonName);

	// Game Descriptions
	mDescriptionFactory->RegisterDescription<CollisionDescription>(CollisionDescription::JsonName);
	mDescriptionFactory->RegisterDescription<InteractiveDescription>(InteractiveDescription::JsonName);
	mDescriptionFactory->RegisterDescription<PhysicsDescription>(PhysicsDescription::JsonName);
	mDescriptionFactory->RegisterDescription<PlayerDescription>(PlayerDescription::JsonName);
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
