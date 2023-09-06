#include "Defines.h"
#include "Entity/Descriptions/CollisionDescription.h"
#include "Entity/Descriptions/PhysicsDescription.h"
#include "Entity/Descriptions/PlayerDescription.h"
#include "Entity/Descriptions/SpriteDescription.h"
#include "Entity/Descriptions/TransformDescription.h"
#include "Graphics/Debug/DebugDraw.h"
#include "GUI/GUISystem.h"

#include "EngineContext.h"

void EngineContext::Init(const char* gameDataFolder)
{
	mConfigManager = std::make_unique<ConfigManager>();
	mConfigManager->LoadConfigFile(gameDataFolder);
	mRenderContext = std::make_unique<RenderContext>();

	mDescriptionFactory = std::make_unique<DescriptionFactory>();
	_InitDescriptions();

	mEventPublisher = std::make_unique<EventPublisher>();
	mEntityRegistry = std::make_unique<EntityRegistry>(*mDescriptionFactory, mConfigManager->GetEntitiesFolderPath(),
	                                                   *mEventPublisher);
	mMapSystem = std::make_unique<MapSystem>(mConfigManager->GetMapsFolderPath());
	mTimer = std::make_unique<Timer>();

	mResourceManager = std::make_unique<ResourceManager>(mConfigManager->GetTexturesFolderPath());

	mInputHandler = std::make_shared<InputHandler>();
	mInputManager = std::make_unique<InputManager>(*mInputHandler);

	// Register exit button
	mInputManager->registerButtonUpHandler(LLGL::Key::Escape, [=]() { mRunning = false; });

	mLevelManager = std::make_unique<LevelManager>(mConfigManager->GetLevelsFolderPath(), *mMapSystem);
	mEditorGUI = std::make_unique<EditorGUI>(*mInputManager, *mLevelManager, *mMapSystem, *mRenderContext,
	                                         *mResourceManager, *mConfigManager);

	// systems
	mCollisionSystem = std::make_unique<CollisionSystem>(*mEventPublisher);
	mPhysicsSystem = std::make_unique<PhysicsSystem>();

	mRenderContext->Init(mConfigManager->GetShadersFolderPath(), *mResourceManager, mInputHandler);
	GUISystem::InitGUI(*mRenderContext);
	mRenderContext->LoadFont(mConfigManager->GetFontFolderPath(), "PixelLettersFull.ttf");

	mRenderContext->DrawText("Allie", {0, 100}, {10, 10});
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

void EngineContext::Run() const
{
	// Init current time
	mTimer->mCurrentTime = Clock::now();

	while (mRenderContext->ProcessEvents() && mRunning)
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
			_FixedUpdate(mTimer->mDT);
			Update(mTimer->mDT);
			mTimer->mAccumulator -= deltaTime;
		}

		_DrawAxis();

		mRenderContext->BeginFrame();
		mRenderContext->Render(mLevelManager->GetCurrentLevel()->GetCamera(), *mEntityRegistry);
		{
			GUISystem::GUIStartFrame();
			mEditorGUI->RenderGUI();
			GUISystem::GUIEndFrame();
		}
		mRenderContext->EndFrame();
	}

	GUISystem::CloseGUI();
}

void EngineContext::_InitDescriptions() const
{
	mDescriptionFactory->RegisterDescription<CollisionDescription>(CollisionDescription::JsonName);
	mDescriptionFactory->RegisterDescription<PhysicsDescription>(PhysicsDescription::JsonName);
	mDescriptionFactory->RegisterDescription<PlayerDescription>(PlayerDescription::JsonName);
	mDescriptionFactory->RegisterDescription<SpriteDescription>(SpriteDescription::JsonName);
	mDescriptionFactory->RegisterDescription<TransformDescription>(TransformDescription::JsonName);

	// Defined by Game 
	RegisterEntityDescriptions();
}

void EngineContext::_FixedUpdate(float dt) const
{
	mPhysicsSystem->Update(dt, *mEntityRegistry);
}
