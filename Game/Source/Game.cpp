#include "Core/Timer.h"
#include "Core/World.h"
#include "Entity/Components/ModelComponent.h"
#include "Entity/Descriptions/AnimationDescription.h"
#include "Entity/Descriptions/CollisionDescription.h"
#include "Entity/Descriptions/InteractiveDescription.h"
#include "Entity/Descriptions/LightDescription.h"
#include "Entity/Descriptions/ModelDescription.h"
#include "Entity/Descriptions/PhysicsDescription.h"
#include "Entity/Descriptions/PlayerDescription.h"
#include "Entity/Descriptions/ScriptDescription.h"
#include "Entity/Descriptions/SpriteDescription.h"
#include "Entity/Descriptions/TransformDescription.h"
#include "Entity/Descriptions/TriggerVolumeDescription.h"
#include "Entity/Systems/MovementSystem.h"
#include "Entity/Systems/PlayerSystem.h"
#include "Graphics/Animation/Animator.h"
#include "Graphics/RenderSystem.h"
#include "GUI/GUISystem.h"
#include "GUI/GUIMenu.h"
#include "Physics/PhysicsSystem.h"
#include "Script/LuaSystem.h"

#include "Game.h"

int main()
{
	Game game = Game(LLGL::Extent2D{1200, 800}, "Game", LLGL::ColorRGBAf{1.0f, 0.5f, 1.0f});

	game.Run();

	return 0;
}

void Game::Run()
{
	mWindowContext->SetBackgroundClearColor({0.1f, 0.1f, 0.1f});
	mRenderSystem->LoadFont("PixelLettersFull.ttf");
	mGUISystem->LoadGUITheme("DarkStyle");

	mSceneManager->LoadScene("menu.json");

	ToggleEditorMode(true);

	// Init current time
	auto timer = new Timer();
	timer->mCurrentTime = Clock::now();

	double frames = 0;
	double fps = 0;
	double fpsTimer = 0.0;

	while (mWindowContext->ProcessEvents() && mRunning)
	{
		auto newTime = Clock::now();
		Duration frameDuration = newTime - timer->mCurrentTime;
		timer->mCurrentTime = newTime;

		double deltaTime = frameDuration.count();
		if (deltaTime > 0.25)
		{
			// Clamp large deltaTime to prevent "spiral of death"
			// where we could potentially always be "catching up"
			deltaTime = 0.25;
		}

		fpsTimer += deltaTime;
		frames++;

		if (fpsTimer >= 1.0)
		{
			fps = frames / fpsTimer;
			frames = 0.0;
			fpsTimer = 0.0;
		}

		timer->mAccumulator += deltaTime;
		if (timer->mAccumulator > timer->mAccumulatorMax)
		{
			timer->mAccumulator = timer->mAccumulatorMax;
		}

		while (timer->mAccumulator >= timer->mDT)
		{
			mSystemManager->FixedUpdate(timer->mDT);
			timer->mAccumulator -= timer->mDT;
		}
		auto world = mSceneManager->GetCurrentWorld();

		if (world)
		{
			world->FlushEvents();
		}

		mInputHandler->Update();
		mSystemManager->Update(deltaTime);

		if (world)
		{
			world->GetCamera().Update();

			mWindowContext->BeginFrame(mRenderSystem->GetCommands());

			mSystemManager->Render(world);

			// Render GUI last so menus draw on top
			mGUISystem->GUIStartFrame();
			mGUISystem->RenderMenus();
			mSystemManager->RenderGUI(deltaTime, fps);
			mGUISystem->GUIEndFrame();

			mWindowContext->EndFrame(mRenderSystem->GetCommands(), mRenderSystem->GetCommandQueue());
		}
	}

	mGUISystem->CloseGUI();
	mResourceManager->Shutdown();
	mSystemManager->Shutdown();
}

Game::Game(const LLGL::Extent2D screenSize, const LLGL::UTF8String& title,
                 const LLGL::ColorRGBAf backgroundClearColor, bool transparent)
{
	
	InitCoreSystems();
	RegisterDescriptions();
	InitDependentSystems(screenSize, title, backgroundClearColor, transparent);
	InitGameplaySystems();
	SetupLuaBindings();
}

void Game::InitCoreSystems()
{
	mSystemManager = std::make_unique<SystemManager>();
	mInputHandler = std::make_shared<InputHandler>();
	mResourceManager = std::make_unique<ResourceManager>();
	mDescriptionFactory = std::make_unique<DescriptionFactory>();
	mEventPublisher = std::make_unique<EventPublisher>();
}

void Game::RegisterDescriptions()
{
	mDescriptionFactory->RegisterDescription<AnimationDescription>(AnimationDescription::JsonName);
	mDescriptionFactory->RegisterDescription<CollisionDescription>(CollisionDescription::JsonName);
	mDescriptionFactory->RegisterDescription<InteractiveDescription>(InteractiveDescription::JsonName);
	mDescriptionFactory->RegisterDescription<LightDescription>(LightDescription::JsonName);
	mDescriptionFactory->RegisterDescription<ModelDescription>(ModelDescription::JsonName);
	mDescriptionFactory->RegisterDescription<PhysicsDescription>(PhysicsDescription::JsonName);
	mDescriptionFactory->RegisterDescription<PlayerDescription>(PlayerDescription::JsonName);
	mDescriptionFactory->RegisterDescription<ScriptDescription>(ScriptDescription::JsonName);
	mDescriptionFactory->RegisterDescription<SpriteDescription>(SpriteDescription::JsonName);
	mDescriptionFactory->RegisterDescription<TransformDescription>(TransformDescription::JsonName);
	mDescriptionFactory->RegisterDescription<TriggerVolumeDescription>(TriggerVolumeDescription::JsonName);
}

// Add an engine level onWOrldCreate event for all systems to subscribe to

void Game::InitDependentSystems(const LLGL::Extent2D screenSize, const LLGL::UTF8String& title,
	const LLGL::ColorRGBAf backgroundClearColor, bool transparent)
{
	mRenderSystem = mSystemManager->RegisterSystem<RenderSystem>();
	mWindowContext = std::make_unique<WindowContext>(*mRenderSystem, screenSize, backgroundClearColor, title, transparent);
	mRenderSystem->OnWindowCreated(*mResourceManager, *mEventPublisher);
	mGUISystem = std::make_shared<GUISystem>(*mWindowContext);
	mInputContext = std::make_unique<InputContext>(*mInputHandler, *mGUISystem);
	mWindowContext->AddEventListener(mInputContext);
	mSceneManager = std::make_unique<SceneManager>(*mDescriptionFactory, *mRenderSystem, *mResourceManager, *mEventPublisher);
	mInputHandler->RegisterButtonUpHandler(LLGL::Key::Escape, [=]() { mRunning = false; });
	mResourceManager->LoadAllResources(*mRenderSystem);
}

void Game::InitGameplaySystems()
{
	// Gameplay systems
	mSystemManager->RegisterSystem<AnimatorSystem>(*mResourceManager, *mEventPublisher);
	const auto physicsSystem = mSystemManager->RegisterSystem<PhysicsSystem>(*mRenderSystem, *mResourceManager, *mEventPublisher);
	mSystemManager->RegisterSystem<MovementSystem>(*physicsSystem, *mEventPublisher);
	mSystemManager->RegisterSystem<PlayerSystem>(mInputHandler, *mEventPublisher);
	mSystemManager->RegisterSystem<EditorGUI>(mInputHandler, *mRenderSystem, *mResourceManager, *mSceneManager, *mWindowContext, *mEventPublisher);
}

void Game::SetupLuaBindings()
{
	const auto luaSystem = mSystemManager->RegisterSystem<LuaSystem>(*mEventPublisher);
	mSceneManager->SetLuaSystem(luaSystem);

	mSceneManager->Bind(luaSystem->GetState());
	mGUISystem->Bind(luaSystem->GetState());

	Entity::Bind(luaSystem->GetState());
	GUIMenu::Bind(luaSystem->GetState());
	GUILayout::Bind(luaSystem->GetState());
}

void Game::ToggleEditorMode(bool toggle)
{
	if (toggle)
	{
		if (mSceneManager->GetCurrentWorld())
		{
			mInputHandler->AddEditorInputs(mSceneManager->GetCurrentWorld()->GetCamera());
		}
	}
}
