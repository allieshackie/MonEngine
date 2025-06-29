#include "LLGL/Types.h"
#include "LLGL/Utils/ColorRGBA.h"
#include "Core/Timer.h"
#include "Entity/Descriptions/AnimationDescription.h"
#include "Entity/Descriptions/CollisionDescription.h"
#include "Entity/Descriptions/InteractDescription.h"
#include "Entity/Descriptions/LightDescription.h"
#include "Entity/Descriptions/OverlayDescription.h"
#include "Entity/Descriptions/ModelDescription.h"
#include "Entity/Descriptions/PhysicsDescription.h"
#include "Entity/Descriptions/PlayerDescription.h"
#include "Entity/Descriptions/SpriteDescription.h"
#include "Entity/Components/OverlayComponent.h"
#include "Entity/Descriptions/TransformDescription.h"
#include "Graphics/Animation/Animator.h"
#include "GUI/GUISystem.h"

#include "App.h"

int main()
{
	const auto app = std::make_unique<App>(LLGL::Extent2D{800, 600}, "",
	                                       LLGL::ColorRGBAf{1.0f, 0.0f, 1.0f, 1.0f}, true, true);

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
		mInteractSystem->Update(*mRenderContext, mSceneManager->GetCamera(), *mPhysicsSystem);

		// TODO: Debug draw axis
		//_DrawAxis();
		// TODO: Debug draw model bones
		//_DebugDrawBones();

		mRenderContext->BeginFrame();

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
		mInteractSystem->Render();
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

void App::_DebugDrawBones() const
{
	const auto modelView = mSceneManager->GetCurrentScene()->GetRegistry().view<const ModelComponent>();

	// Should either be a sprite or basic color
	modelView.each([=](const ModelComponent& modelComp)
	{
		auto model = mResourceManager->GetModelFromId(modelComp.mModelPath);
		_RenderModelBones(model, modelComp, model.GetRootNodeIndex(), glm::mat4(1.0f));
	});
}

void App::_RenderModelBones(Model& model, const ModelComponent& modelComp, int nodeIndex,
                            const glm::mat4 parentTransform) const
{
	const auto node = model.GetJointNodeAt(nodeIndex);

	if (node == nullptr)
	{
		return;
	}

	glm::mat4 globalTransform = parentTransform * node->mTransformation;
	auto bonePosition = glm::vec3(globalTransform[3]);
	auto parentPosition = glm::vec3(parentTransform[3]);

	// Default length for visualization purposes
	float defaultLength = 1.0f; // Adjust this as needed
	glm::vec3 direction = glm::normalize(bonePosition - parentPosition) * defaultLength;
	glm::vec3 endPosition = bonePosition + direction;

	glm::vec4 color = {1, 1, 1, 1};
	if (strcmp(node->mId.c_str(), "Head") == 0)
	{
		color = {1, 0, 0, 1};
	}

	mRenderContext->DrawLine(bonePosition, endPosition, color);

	for (const auto child : node->mChildren)
	{
		_RenderModelBones(model, modelComp, child, globalTransform);
	}
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
	mDescriptionFactory->RegisterDescription<InteractDescription>(InteractDescription::JsonName);
	mDescriptionFactory->RegisterDescription<LightDescription>(LightDescription::JsonName);
	mDescriptionFactory->RegisterDescription<ModelDescription>(ModelDescription::JsonName);
	mDescriptionFactory->RegisterDescription<OverlayDescription>(OverlayDescription::JsonName);
	mDescriptionFactory->RegisterDescription<PhysicsDescription>(PhysicsDescription::JsonName);
	mDescriptionFactory->RegisterDescription<PlayerDescription>(PlayerDescription::JsonName);
	mDescriptionFactory->RegisterDescription<SpriteDescription>(SpriteDescription::JsonName);
	mDescriptionFactory->RegisterDescription<TransformDescription>(TransformDescription::JsonName);

	// Init all systems **with** dependencies
	mAnimator = std::make_unique<Animator>(*mResourceManager);
	mSceneManager = std::make_unique<SceneManager>(*mDescriptionFactory);
	mPhysicsSystem = std::make_unique<PhysicsSystem>(*mRenderContext);
	mInteractSystem = std::make_unique<InteractSystem>(*mInputHandler, *mSceneManager);

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

	EventFunc func = [this](Entity* entity)
	{
		mRenderContext->DrawOverlay({1, 1}, {1, 1, 1, 0});
	};
	mSceneManager->ConnectOnConstruct<OverlayComponent>(func);
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
