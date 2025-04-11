#include "LLGL/Types.h"
#include "LLGL/Utils/ColorRGBA.h"
#include "Core/Timer.h"
#include "Entity/Descriptions/AnimationDescription.h"
#include "Entity/Descriptions/CollisionDescription.h"
#include "Entity/Descriptions/LightDescription.h"
#include "Entity/Descriptions/MeshDescription.h"
#include "Entity/Descriptions/PhysicsDescription.h"
#include "Entity/Descriptions/PlayerDescription.h"
#include "Entity/Descriptions/SpriteDescription.h"
#include "Entity/Descriptions/TransformDescription.h"
#include "Graphics/Animation/Animator.h"
#include "GUI/GUISystem.h"

#include "App.h"

int main()
{
	const auto app = std::make_unique<App>(LLGL::Extent2D{800, 600}, "",
	                                       LLGL::ColorRGBAf{0.0f, 0.0f, 0.0f, 0.0f}, true, true);

	app->Run();

	return 0;
}


void App::Run() const
{
	// TODO: Re-add font, make sure to have font for gui
	//mEngine->LoadFont("PixelLettersFull.ttf");

	// TODO: Data drive theming to make it easier to have different styles
	//GUISystem::LoadGUITheme();
	mSceneManager->LoadScene("app.json", *mMapRegistry, *mLuaSystem);
	ToggleEditorMode(true);

	// Init current time
	auto timer = new Timer();
	timer->mCurrentTime = Clock::now();

	while (mRenderContext->ProcessEvents() && mRunning)
	{
		auto frameTime = Clock::now();
		Duration elapsed = frameTime - timer->mCurrentTime;
		timer->mCurrentTime = frameTime;

		double deltaTime = elapsed.count();
		if (deltaTime > 0.25)
		{
			// Clamp large deltaTime to prevent "spiral of death"
			// where we could potentially always be "catching up"
			deltaTime = 0.25;
		}

		timer->mAccumulator += deltaTime;

		while (timer->mAccumulator >= deltaTime)
		{
			_FixedUpdate(timer->mDT);
			timer->mAccumulator -= timer->mDT;
		}

		mInputHandler->Update();
		mSceneManager->GetCamera().Update();
		mAnimator->Update(deltaTime, mSceneManager->GetCurrentScene(), *mResourceManager);

		// TODO: Debug draw axis
		//_DrawAxis();
		// TODO: Debug draw model bones
		//_DebugDrawBones();

		mRenderContext->BeginFrame();

		//game->Render();

		if (mSceneManager->GetCurrentScene())
		{
			mRenderContext->Render(mSceneManager->GetCamera(), mSceneManager->GetCurrentScene(), *mResourceManager);
		}

		// Render GUI last so menus draw on top
		GUISystem::GUIStartFrame();
		GUISystem::RenderMenus();
		// TODO: Debug Gui menu
		//GUISystem::RenderGuiElements();
		if (mEditorGUI != nullptr)
		{
			mEditorGUI->Render(mSceneManager->GetCurrentScene(), *mResourceManager, *mRenderContext, *mInputHandler);
		}
		GUISystem::GUIEndFrame();

		mRenderContext->EndFrame();

		if (mLuaSystem->QueueCloseScripts())
		{
			mLuaSystem->CloseAllScripts();
		}
	}

	GUISystem::CloseGUI();
}

void App::SetGUIMenu(std::unique_ptr<GUIBase> gui)
{
	mGUIMenu = std::move(gui);
}

void App::_FixedUpdate(float dt) const
{
	mPhysicsSystem->Update(dt, mSceneManager->GetCurrentScene(), *mResourceManager);
}

App::App(const LLGL::Extent2D screenSize, const LLGL::UTF8String& title,
         const LLGL::ColorRGBAf backgroundClearColor, bool usePerspective, bool transparent)
{
	// Init all systems **without** dependencies
	mInputHandler = std::make_shared<InputHandler>();
	mResourceManager = std::make_unique<ResourceManager>();
	mDescriptionFactory = std::make_unique<DescriptionFactory>();
	mEventPublisher = std::make_unique<EventPublisher>();
	mMapRegistry = std::make_unique<MapRegistry>();
	mLuaSystem = std::make_unique<LuaSystem>();
	mRenderContext = std::make_unique<RenderContext>(screenSize, backgroundClearColor, usePerspective);

	// Must be called before SceneManager sets up description factory
	mDescriptionFactory->RegisterDescription<AnimationDescription>(AnimationDescription::JsonName);
	mDescriptionFactory->RegisterDescription<CollisionDescription>(CollisionDescription::JsonName);
	mDescriptionFactory->RegisterDescription<LightDescription>(LightDescription::JsonName);
	mDescriptionFactory->RegisterDescription<MeshDescription>(MeshDescription::JsonName);
	mDescriptionFactory->RegisterDescription<PhysicsDescription>(PhysicsDescription::JsonName);
	mDescriptionFactory->RegisterDescription<PlayerDescription>(PlayerDescription::JsonName);
	mDescriptionFactory->RegisterDescription<SpriteDescription>(SpriteDescription::JsonName);
	mDescriptionFactory->RegisterDescription<TransformDescription>(TransformDescription::JsonName);

	// Init all systems **with** dependencies
	mAnimator = std::make_unique<Animator>(*mResourceManager);
	mSceneManager = std::make_unique<SceneManager>(*mDescriptionFactory);
	mPhysicsSystem = std::make_unique<PhysicsSystem>(*mRenderContext);

	// Remaining system setup
	mInputHandler->RegisterButtonUpHandler(LLGL::Key::Escape, [=]() { mRunning = false; });
	GUISystem::InitGUI(*mRenderContext);
	mResourceManager->LoadAllResources(mRenderContext->GetRenderSystem());
	mRenderContext->InitPipelines(title, mInputHandler, *mResourceManager, transparent);

	// TODO: Comment/uncomment for editor gui menu
	mEditorGUI = std::make_unique<EditorGUI>(*mSceneManager, *mInputHandler);

	// loading new scene means setting up callbacks for various systems
	mAnimator->SetSceneCallbacks(*mSceneManager);
	mPhysicsSystem->SetSceneCallbacks(*mSceneManager);
	mRenderContext->SetSceneCallbacks(*mSceneManager);
}

void App::ToggleEditorMode(bool toggle) const
{
	if (toggle)
	{
		if (mSceneManager->GetCurrentScene())
		{
			mInputHandler->AddEditorInputs(mSceneManager->GetCamera());
		}
	}
}
