#include "Core/Timer.h"
#include "Entity/Components/ModelComponent.h"
#include "Entity/Descriptions/AnimationDescription.h"
#include "Entity/Descriptions/CollisionDescription.h"
#include "Entity/Descriptions/InteractiveDescription.h"
#include "Entity/Descriptions/LightDescription.h"
#include "Entity/Descriptions/ModelDescription.h"
#include "Entity/Descriptions/PhysicsDescription.h"
#include "Entity/Descriptions/PlayerDescription.h"
#include "Entity/Descriptions/SpriteDescription.h"
#include "Entity/Descriptions/TransformDescription.h"
#include "Graphics/Core/Node.h"
#include "Graphics/Animation/Animator.h"
#include "Graphics/RenderSystem.h"
#include "GUI/GUISystem.h"

#include "Sandbox.h"

int main()
{
	const auto game = std::make_unique<Sandbox>(LLGL::Extent2D{800, 600}, "Sandbox",
	                                            LLGL::ColorRGBAf{1.0f, 0.5f, 1.0f});

	game->Run();

	return 0;
}

void Sandbox::Run()
{
	// TODO: Re-add font, make sure to have font for gui
	//mRenderSystem->LoadFont("PixelLettersFull.ttf");
	mRenderContext->SetBackgroundClearColor({0.1f, 0.1f, 0.1f});
	// TODO: Data drive theming to make it easier to have different styles
	//GUISystem::LoadGUITheme();
	mSceneManager->LoadScene("game.json", *mMapRegistry, *mLuaSystem);

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
			mSystemManager->FixedUpdate(timer->mDT);
			timer->mAccumulator -= timer->mDT;
		}

		mInputHandler->Update();
		mSystemManager->Update(timer->mDT);

		auto world = mSceneManager->GetCurrentWorld();
		if (world)
		{
			world->GetCamera().Update();

			// TODO: Debug draw axis
			_DrawAxis();
			// TODO: Debug draw model bones
			//_DebugDrawBones();

			mRenderContext->BeginFrame();

			mSystemManager->Render(world);

			// Render GUI last so menus draw on top
			GUISystem::GUIStartFrame();
			GUISystem::RenderMenus();
			// TODO: Debug Gui menu
			//GUISystem::RenderGuiElements();

			mSystemManager->RenderGUI();

			GUISystem::GUIEndFrame();

			mRenderContext->EndFrame();
		}

		if (mLuaSystem->QueueCloseScripts())
		{
			mLuaSystem->CloseAllScripts();
		}
	}

	GUISystem::CloseGUI();
}

void Sandbox::SetGUIMenu(std::unique_ptr<GUIBase> gui)
{
	mGUIMenu = std::move(gui);
}

Sandbox::Sandbox(const LLGL::Extent2D screenSize, const LLGL::UTF8String& title,
                 const LLGL::ColorRGBAf backgroundClearColor, bool usePerspective, bool transparent)
{
	mSystemManager = std::make_unique<SystemManager>();
	mInputHandler = std::make_shared<InputHandler>();

	// Init all systems **without** dependencies
	mResourceManager = std::make_unique<ResourceManager>();
	mDescriptionFactory = std::make_unique<DescriptionFactory>();
	mEventPublisher = std::make_unique<EventPublisher>();
	mMapRegistry = std::make_unique<MapRegistry>();
	mLuaSystem = std::make_unique<LuaSystem>();
	mRenderContext = std::make_unique<RenderContext>(screenSize, backgroundClearColor, usePerspective, title,
	                                                 mInputHandler, transparent);

	// Must be called before SceneManager sets up description factory
	mDescriptionFactory->RegisterDescription<AnimationDescription>(AnimationDescription::JsonName);
	mDescriptionFactory->RegisterDescription<CollisionDescription>(CollisionDescription::JsonName);
	mDescriptionFactory->RegisterDescription<InteractiveDescription>(InteractiveDescription::JsonName);
	mDescriptionFactory->RegisterDescription<LightDescription>(LightDescription::JsonName);
	mDescriptionFactory->RegisterDescription<ModelDescription>(ModelDescription::JsonName);
	mDescriptionFactory->RegisterDescription<PhysicsDescription>(PhysicsDescription::JsonName);
	mDescriptionFactory->RegisterDescription<PlayerDescription>(PlayerDescription::JsonName);
	mDescriptionFactory->RegisterDescription<SpriteDescription>(SpriteDescription::JsonName);
	mDescriptionFactory->RegisterDescription<TransformDescription>(TransformDescription::JsonName);

	// Init all systems **with** dependencies
	mSceneManager = std::make_unique<SceneManager>(*mDescriptionFactory);

	// Remaining system setup
	mInputHandler->RegisterButtonUpHandler(LLGL::Key::Escape, [=]() { mRunning = false; });
	GUISystem::InitGUI(*mRenderContext);
	mResourceManager->LoadAllResources(mRenderContext->GetRenderSystem());

	auto world = mSceneManager->GetCurrentWorld();
	mRenderSystem = mSystemManager->RegisterSystem<RenderSystem>(*mRenderContext, *mResourceManager, world);
	// Gameplay systems
	mSystemManager->RegisterSystem<AnimatorSystem>(*mResourceManager, world);
	auto physicsSystem = mSystemManager->RegisterSystem<PhysicsSystem>(*mRenderSystem, *mResourceManager, world);
	mSystemManager->RegisterSystem<MovementSystem>(*physicsSystem, world);
	mSystemManager->RegisterSystem<PlayerSystem>(mInputHandler, world);
	mSystemManager->RegisterSystem<EditorGUI>(mInputHandler, world, *mRenderContext, *mResourceManager);
}

void Sandbox::ToggleEditorMode(bool toggle) const
{
	if (toggle)
	{
		if (mSceneManager->GetCurrentWorld())
		{
			mInputHandler->AddEditorInputs(mSceneManager->GetCurrentWorld()->GetCamera());
		}
	}
}

void Sandbox::_DrawAxis() const
{
	mRenderSystem->ClearOverlay();
	const glm::mat4& view = mSceneManager->GetCurrentWorld()->GetCamera().GetView();
	auto cameraRotation = glm::mat3(glm::inverse(view));
	glm::vec3 X = cameraRotation * glm::vec3{1, 0, 0};
	glm::vec3 Y = cameraRotation * glm::vec3{0, 1, 0};
	glm::vec3 Z = cameraRotation * glm::vec3{0, 0, 1};

	auto origin = glm::vec2(0, 0); // bottom-left corner in pixels
	float axisLength = 5.0;

	auto project = [&](const glm::vec3& v)
	{
		return origin + glm::vec2(v.x, -v.y) * axisLength; // note Y flip
	};

	glm::vec2 xEnd = project(X);
	glm::vec2 yEnd = project(Y);
	glm::vec2 zEnd = project(Z);
	// Draw the XYZ axis at 0,0,0
	// X Axis: Red.  Box on positive end
	mRenderSystem->DrawOverlayLine(origin, xEnd, {255, 0, 0, 1});
	// Y Axis: Green
	//mRenderSystem->DrawOverlayLine(origin, yEnd, {0, 255, 0, 1});
	// Z Axis: Blue
	//mRenderSystem->DrawOverlayLine(origin, zEnd, {0, 0, 255, 1});

	// Draw Floor Grid
	//mRenderSystem->DrawGrid();
}

void Sandbox::_DebugDrawBones() const
{
	const auto modelView = mSceneManager->GetCurrentWorld()->GetRegistry().view<const ModelComponent>();

	// Should either be a sprite or basic color
	modelView.each([=](const ModelComponent& modelComp)
	{
		auto model = mResourceManager->GetModelFromId(modelComp.mModelPath);
		_RenderModelBones(model, modelComp, model.GetRootNodeIndex(), glm::mat4(1.0f));
	});
}

void Sandbox::_RenderModelBones(Model& model, const ModelComponent& modelComp, int nodeIndex,
                                const glm::mat4 parentTransform) const
{
	const auto node = model.GetNodeAt(nodeIndex);

	if (node == nullptr)
	{
		return;
	}

	const auto jointNode = model.GetJointDataAt(node->mJointIndex);

	glm::mat4 globalTransform = parentTransform * node->mTransform;
	auto bonePosition = glm::vec3(globalTransform[3]);
	auto parentPosition = glm::vec3(parentTransform[3]);

	// Default length for visualization purposes
	float defaultLength = 1.0f; // Adjust this as needed
	glm::vec3 direction = glm::normalize(bonePosition - parentPosition) * defaultLength;
	glm::vec3 endPosition = bonePosition + direction;

	glm::vec4 color = {1, 1, 1, 1};
	if (strcmp(jointNode->mId.c_str(), "Head") == 0)
	{
		color = {1, 0, 0, 1};
	}

	mRenderSystem->DrawLine(bonePosition, endPosition, color);

	for (const auto child : node->mChildren)
	{
		_RenderModelBones(model, modelComp, child, globalTransform);
	}
}
