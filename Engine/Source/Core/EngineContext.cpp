#include "Editor/EditorGUI.h"
#include "Entity/Descriptions/CollisionDescription.h"
#include "Entity/Descriptions/PhysicsDescription.h"
#include "Entity/Descriptions/PlayerDescription.h"
#include "Entity/Descriptions/SpriteDescription.h"
#include "Entity/Descriptions/TransformDescription.h"
#include "GameInterface.h"
#include "GUI/GUISystem.h"

#include "EngineContext.h"

void EngineContext::_Init()
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

	// TODO: Have game load font, should have a fallback if no font provided
	//mRenderContext->LoadFont("PixelLettersFull.ttf");

#ifndef BUILD_GAME
	mGUIMenu = std::make_unique<EditorGUI>(*this, *mInputHandler, *mLevelManager, *mMapRegistry, *mRenderContext);
#endif
}

void EngineContext::UseGUIModule()
{
	mUseGUIModule = true;
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
	DrawLine({-1, 0, 0}, {1, 0, 0}, {255, 0, 0});
	DrawBox({1, 0, 0}, {0.1f, 0.1f, 1.0f}, {255, 0, 0});
	DrawLine({0, -1, 0}, {0, 1, 0}, {0, 255, 0});
	DrawBox({0, 1, 0}, {0.1f, 0.1f, 1.0f}, {0, 255, 0});
	DrawLine({0, 0, -1}, {0, 0, 1}, {0, 0, 255});
	DrawBox({0, 0, 1}, {0.1f, 0.1f, 1.0f}, {0, 0, 255});
}

void EngineContext::_InitDescriptions() const
{
	mDescriptionFactory->RegisterDescription<CollisionDescription>(CollisionDescription::JsonName);
	mDescriptionFactory->RegisterDescription<PhysicsDescription>(PhysicsDescription::JsonName);
	mDescriptionFactory->RegisterDescription<PlayerDescription>(PlayerDescription::JsonName);
	mDescriptionFactory->RegisterDescription<SpriteDescription>(SpriteDescription::JsonName);
	mDescriptionFactory->RegisterDescription<TransformDescription>(TransformDescription::JsonName);
}

void EngineContext::_FixedUpdate(float dt) const
{
	mPhysicsSystem->Update(dt, *mEntityRegistry);
}

void EngineContext::Run(GameInterface* game)
{
	_Init();
	game->Init(this);
	game->RegisterEntityDescriptions();

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
			game->Update(mTimer->mDT);
			mTimer->mAccumulator -= deltaTime;
		}

		// TODO: Debug draw axis
		//_DrawAxis();

		// TODO: Editor GUI requires camera to render...how can we fix that?
		mRenderContext->BeginFrame();
		if (mUseGUIModule)
		{
			GUISystem::GUIStartFrame();
			mGUIMenu->RenderGUI();
			GUISystem::GUIEndFrame();
		}

		if (const auto& level = mLevelManager->GetCurrentLevel())
		{
			mRenderContext->Render(level->GetCamera(), *mEntityRegistry, *mMapRegistry);
		}

		mRenderContext->EndFrame();
	}

	GUISystem::CloseGUI();
}

void EngineContext::LoadFont(const char* fontFileName) const
{
	mRenderContext->LoadFont(fontFileName);
}

EntityId EngineContext::CreateGameObject(const std::string& entityTemplateName) const
{
	return mEntityRegistry->CreateEntityFromTemplate(entityTemplateName.c_str());
}

EntityRegistry& EngineContext::GetEntityRegistry() const
{
	return *mEntityRegistry;
}

InputHandler& EngineContext::GetInputHandler() const
{
	return *mInputHandler;
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

void EngineContext::DrawPoint(glm::vec3 position, float size, glm::vec3 color) const
{
	mRenderContext->DrawPoint(position, color, size);
}

void EngineContext::DrawLine(glm::vec3 from, glm::vec3 to, glm::vec3 color) const
{
	mRenderContext->DrawLine(from, to, color);
}

void EngineContext::DrawBox(glm::vec3 position, glm::vec3 size, glm::vec3 color) const
{
	mRenderContext->DrawBox(position, size, color);
}

void EngineContext::DrawCircle(glm::vec3 position, float radius, glm::vec3 color) const
{
	mRenderContext->DrawCircle(position, radius, color);
}

void EngineContext::DrawGrid(glm::vec3 position, glm::vec3 size, int rows, int columns, glm::vec3 color) const
{
	mRenderContext->DrawGrid(position, size, color, rows, columns);
}

void EngineContext::DrawText2D(const char* text, glm::vec2 position, glm::vec2 size) const
{
	mRenderContext->DrawText(text, position, size);
}

void EngineContext::DrawText3D(const char* text, glm::vec3 position, glm::vec3 size) const
{
}
