#include "Editor/EditorGUI.h"
#include "Entity/Descriptions/CollisionDescription.h"
#include "Entity/Descriptions/PhysicsDescription.h"
#include "Entity/Descriptions/PlayerDescription.h"
#include "Entity/Descriptions/SpriteDescription.h"
#include "Entity/Descriptions/TransformDescription.h"
#include "Graphics/Debug/DebugDraw.h"
#include "GUI/GUIBase.h"
#include "GUI/GUISystem.h"

#include "EngineContext.h"


void EngineContext::Init()
{
	mInputHandler = std::make_shared<InputHandler>();
	mInputHandler->RegisterButtonUpHandler(LLGL::Key::Escape, [=]() { mRunning = false; });

	mResourceManager = std::make_unique<ResourceManager>();
	mRenderContext = std::make_unique<RenderContext>();
	mRenderContext->Init(mInputHandler);
	GUISystem::InitGUI(*mRenderContext);

	mDescriptionFactory = std::make_unique<DescriptionFactory>();
	_InitDescriptions();

	mEventPublisher = std::make_unique<EventPublisher>();
	mEntityRegistry = std::make_unique<EntityRegistry>(*mDescriptionFactory, *mEventPublisher);
	mMapRegistry = std::make_unique<MapRegistry>();
	mTimer = std::make_unique<Timer>();

	mLevelManager = std::make_unique<LevelManager>(*mMapRegistry);

	mCollisionSystem = std::make_unique<CollisionSystem>(*mEventPublisher);
	mPhysicsSystem = std::make_unique<PhysicsSystem>();

	// TODO: Example
	mRenderContext->LoadFont("PixelLettersFull.ttf");
	mRenderContext->DrawText("Allie", {0, 0}, {1, 1});

#ifndef BUILD_GAME
	mGUIMenu = std::make_unique<EditorGUI>(*this, *mInputHandler, *mLevelManager, *mMapRegistry, *mRenderContext);
#endif
}

void EngineContext::SetGUIMenu(std::unique_ptr<GUIBase> gui)
{
	mGUIMenu = std::move(gui);
}

/*  
 * Draw the XYZ axis at 0,0,0
 * Note: Not ideal for debugging since the axises are drawn in world
 * space and don't follow the camera
 * Ideally, this could be draw directly to screen space and remain in the view
 * like UI
 */
void EngineContext::_DrawAxis() const
{
	if (mDebugDraw)
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

		// TODO: Editor GUI requires camera to render...how can we fix that?
		mRenderContext->BeginFrame();
		{
			GUISystem::GUIStartFrame();
			mGUIMenu->RenderGUI();
			GUISystem::GUIEndFrame();
		}

		if (const auto& level = mLevelManager->GetCurrentLevel())
		{
			mRenderContext->Render(level->GetCamera(), *mEntityRegistry);
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

void EngineContext::LoadFont(const char* fontFileName) const
{
	mRenderContext->LoadFont(fontFileName);
}

EntityId EngineContext::CreateGameObject(const std::string& entityTemplateName) const
{
	return mEntityRegistry->CreateEntityFromTemplate(entityTemplateName.c_str());
}

void EngineContext::LoadLevel(const char* levelName) const
{
	mLevelManager->LoadLevel(levelName, *this);
}

const std::unique_ptr<Level>& EngineContext::GetLevel() const
{
	return mLevelManager->GetCurrentLevel();
}

void EngineContext::OpenMap(const char* mapName) const
{
	mMapRegistry->OpenMap(mapName);
}

void EngineContext::OpenMap(const char* mapName, glm::vec3 position, glm::vec3 rotation, float tileSize) const
{
	mMapRegistry->OpenMap(mapName, position, rotation, tileSize);
}

void EngineContext::DrawPoint(glm::vec3 position, float size)
{
	DebugDrawManager::GetInstance()->DrawPoint(position, {1, 0, 0}, size);
}

void EngineContext::DrawBox(glm::vec3 position, glm::vec3 size)
{
	DebugDrawManager::GetInstance()->DrawBox(position, {1, 0, 0}, size);
}

void EngineContext::DrawGrid(glm::vec3 position, glm::vec3 size, int rows, int columns)
{
	DebugDrawManager::GetInstance()->DrawGrid(position, size, {1, 0, 0}, rows, columns);
}

void EngineContext::DrawText2D(const char* text, glm::vec2 position, glm::vec2 size)
{
}

void EngineContext::DrawText3D(const char* text, glm::vec3 position, glm::vec3 size)
{
}
