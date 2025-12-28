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
#include "Physics/PhysicsSystem.h"
#include "Script/LuaSystem.h"

#include "Sandbox.h"

int main()
{
	const auto game = std::make_unique<Sandbox>(LLGL::Extent2D{1200, 800}, "Sandbox",
	                                            LLGL::ColorRGBAf{1.0f, 0.5f, 1.0f});

	game->Run();

	return 0;
}

void Sandbox::Run()
{
	mRenderContext->SetBackgroundClearColor({0.1f, 0.1f, 0.1f});
	//mRenderSystem->LoadFont("PixelLettersFull.ttf");
	//GUISystem::LoadGUITheme("LightStyle");
	mSceneManager->LoadScene("menu.json");

	ToggleEditorMode(true);

	// Init current time
	auto timer = new Timer();
	timer->mCurrentTime = Clock::now();

	double frames = 0;
	double fps = 0;

	while (mRenderContext->ProcessEvents() && mRunning)
	{
		auto currentTime = Clock::now();
		Duration elapsed = currentTime - timer->mCurrentTime;

		double deltaTime = elapsed.count();
		if (deltaTime > 0.25)
		{
			// Clamp large deltaTime to prevent "spiral of death"
			// where we could potentially always be "catching up"
			deltaTime = 0.25;
		}

		frames++;
		if (elapsed.count() >= 1.0)
		{
			fps = frames / elapsed.count();
			frames = 0;
			timer->mCurrentTime = currentTime;
		}

		timer->mAccumulator += deltaTime;

		while (timer->mAccumulator >= deltaTime)
		{
			mSystemManager->FixedUpdate(timer->mDT);
			timer->mAccumulator -= timer->mDT;
		}

		mInputHandler->Update();
		mSystemManager->Update(timer->mDT);

		auto world = mSceneManager->GetCurrentWorld();
		if (world)
		{
			world->GetCamera().Update();

			mRenderContext->BeginFrame();

			mSystemManager->Render(world);

			// Render GUI last so menus draw on top
			mGUISystem->GUIStartFrame();
			if (ImGui::Begin("Test"))
			{
				ImGui::Text("FPS: %.1f", fps);
			}
			ImGui::End();
			//mGUISystem->RenderMenus();
			//GUISystem::RenderGuiElements(); // Demo menu

			mSystemManager->RenderGUI();

			mGUISystem->GUIEndFrame();

			mRenderContext->EndFrame();
		}
	}

	mGUISystem->CloseGUI();
}

Sandbox::Sandbox(const LLGL::Extent2D screenSize, const LLGL::UTF8String& title,
                 const LLGL::ColorRGBAf backgroundClearColor, bool transparent)
{
	// Init all systems **without** dependencies
	mSystemManager = std::make_unique<SystemManager>();
	mInputHandler = std::make_shared<InputHandler>();
	mResourceManager = std::make_unique<ResourceManager>();
	mDescriptionFactory = std::make_unique<DescriptionFactory>();
	mEventPublisher = std::make_unique<EventPublisher>();
	mMapRegistry = std::make_unique<MapRegistry>();

	// Must be called before SceneManager sets up description factory
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

	// Init all systems **with** dependencies
	mRenderContext = std::make_unique<RenderContext>(screenSize, backgroundClearColor, title, mInputHandler,
	                                                 transparent);
	mGUISystem = std::make_shared<GUISystem>(*mRenderContext);
	mSceneManager = std::make_unique<SceneManager>(*mDescriptionFactory, *mMapRegistry);

	// Remaining system setup
	mInputHandler->SetGUISystem(mGUISystem);
	mInputHandler->RegisterButtonUpHandler(LLGL::Key::Escape, [=]() { mRunning = false; });
	mResourceManager->LoadAllResources(mRenderContext->GetRenderSystem());

	auto world = mSceneManager->GetCurrentWorld();
	mRenderSystem = mSystemManager->RegisterSystem<RenderSystem>(*mRenderContext, *mResourceManager, world);
	// Gameplay systems
	mSystemManager->RegisterSystem<AnimatorSystem>(*mResourceManager, world);
	const auto physicsSystem = mSystemManager->RegisterSystem<PhysicsSystem>(*mRenderSystem, *mResourceManager, world);
	mSystemManager->RegisterSystem<MovementSystem>(*physicsSystem, world);
	mSystemManager->RegisterSystem<PlayerSystem>(mInputHandler, world);
	mSystemManager->RegisterSystem<EditorGUI>(mInputHandler, world, *mRenderContext, *mResourceManager, *mRenderSystem);
	const auto luaSystem = mSystemManager->RegisterSystem<LuaSystem>(world);

	mSceneManager->SetLuaSystem(luaSystem);

	mSceneManager->Bind(luaSystem->GetState());
	mGUISystem->Bind(luaSystem->GetState());

	// Slightly different since there will be multiple instances
	Entity::Bind(luaSystem->GetState());
}

void Sandbox::ToggleEditorMode(bool toggle)
{
	if (toggle)
	{
		if (mSceneManager->GetCurrentWorld())
		{
			mInputHandler->AddEditorInputs(mSceneManager->GetCurrentWorld()->GetCamera());
		}
	}
}
