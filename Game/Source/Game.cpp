#include "Core/Timer.h"
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
	//GUISystem::LoadGUITheme("LightStyle");

	mSceneManager->LoadScene("menu.json");
	mRenderSystem->OnWorldCreated(mSceneManager->GetCurrentWorld());

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

		mInputHandler->Update();
		mSystemManager->Update(deltaTime);

		auto world = mSceneManager->GetCurrentWorld();
		if (world)
		{
			world->GetCamera().Update();

			mWindowContext->BeginFrame(mRenderSystem->GetCommands());

			mSystemManager->Render(world);

			// Render GUI last so menus draw on top
			mGUISystem->GUIStartFrame();
			if (ImGui::Begin("Test"))
			{
				ImGui::Text("FPS: %.1f", fps);
			}
			ImGui::End();

			mGUISystem->RenderMenus();
			mSystemManager->RenderGUI();
			mGUISystem->GUIEndFrame();

			mWindowContext->EndFrame(mRenderSystem->GetCommands(), mRenderSystem->GetCommandQueue());
		}
	}

	mGUISystem->CloseGUI();
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
	mTerrainSystem = std::make_unique<TerrainSystem>();
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
}

void Game::InitDependentSystems(const LLGL::Extent2D screenSize, const LLGL::UTF8String& title,
	const LLGL::ColorRGBAf backgroundClearColor, bool transparent)
{
	mRenderSystem = mSystemManager->RegisterSystem<RenderSystem>();
	mWindowContext = std::make_unique<WindowContext>(*mRenderSystem, screenSize, backgroundClearColor, title, mInputHandler, transparent);
	mRenderSystem->OnWindowCreated(*mResourceManager);
	mGUISystem = std::make_shared<GUISystem>(*mWindowContext);
	mSceneManager = std::make_unique<SceneManager>(*mDescriptionFactory, *mTerrainSystem, *mResourceManager);
	mInputHandler->SetGUISystem(mGUISystem);
	mInputHandler->RegisterButtonUpHandler(LLGL::Key::Escape, [=]() { mRunning = false; });
	mResourceManager->LoadAllResources(*mRenderSystem);
}

void Game::InitGameplaySystems()
{
	auto world = mSceneManager->GetCurrentWorld();
	// Gameplay systems
	mSystemManager->RegisterSystem<AnimatorSystem>(*mResourceManager, world);
	const auto physicsSystem = mSystemManager->RegisterSystem<PhysicsSystem>(*mRenderSystem, *mResourceManager, world);
	mSystemManager->RegisterSystem<MovementSystem>(*physicsSystem, world);
	mSystemManager->RegisterSystem<PlayerSystem>(mInputHandler, world);
	mSystemManager->RegisterSystem<EditorGUI>(mInputHandler, world, *mWindowContext, *mResourceManager, *mRenderSystem);
}

void Game::SetupLuaBindings()
{
	auto world = mSceneManager->GetCurrentWorld();
	const auto luaSystem = mSystemManager->RegisterSystem<LuaSystem>(world);
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
