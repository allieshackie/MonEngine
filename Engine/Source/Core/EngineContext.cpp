#include "Entity/Descriptions/CollisionDescription.h"
#include "Entity/Descriptions/MeshDescription.h"
#include "Entity/Descriptions/PhysicsDescription.h"
#include "Entity/Descriptions/PlayerDescription.h"
#include "Entity/Descriptions/SpriteDescription.h"
#include "Entity/Descriptions/TransformDescription.h"
#include "GameInterface.h"
#include "GUI/GUISystem.h"

#include "EngineContext.h"

void EngineContext::_Init(const LLGL::Extent2D screenSize, const LLGL::UTF8String& title,
                          const LLGL::ColorRGBAf backgroundClearColor, bool usePerspective)
{
	mInputHandler = std::make_shared<InputHandler>();
	mInputHandler->RegisterButtonUpHandler(LLGL::Key::Escape, [=]() { mRunning = false; });

	mResourceManager = std::make_unique<ResourceManager>();
	mRenderContext = std::make_unique<RenderContext>(title, screenSize, backgroundClearColor, mInputHandler,
	                                                 usePerspective);
	GUISystem::InitGUI(*mRenderContext);
	mLuaSystem = std::make_unique<LuaSystem>();

	mDescriptionFactory = std::make_unique<DescriptionFactory>();
	_InitDescriptions();

	mEventPublisher = std::make_unique<EventPublisher>();
	mEntityRegistry = std::make_unique<EntityRegistry>(*mDescriptionFactory, *mEventPublisher);
	mMapRegistry = std::make_unique<MapRegistry>();
	mTimer = std::make_unique<Timer>();

	mLevelManager = std::make_unique<LevelManager>();

	mCollisionSystem = std::make_unique<CollisionSystem>(*mEventPublisher);
	mPhysicsSystem = std::make_unique<PhysicsSystem>();
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
	DrawLine({-1, 0, 0}, {1, 0, 0}, {255, 0, 0, 1});
	DrawBox({1, 0, 0}, {0.1f, 0.1f, 1.0f}, {255, 0, 0, 1});
	DrawLine({0, -1, 0}, {0, 1, 0}, {0, 255, 0, 1});
	DrawBox({0, 1, 0}, {0.1f, 0.1f, 1.0f}, {0, 255, 0, 1});
	DrawLine({0, 0, -1}, {0, 0, 1}, {0, 0, 255, 1});
	DrawBox({0, 0, 1}, {0.1f, 0.1f, 1.0f}, {0, 0, 255, 1});
}

void EngineContext::_InitDescriptions() const
{
	mDescriptionFactory->RegisterDescription<CollisionDescription>(CollisionDescription::JsonName);
	mDescriptionFactory->RegisterDescription<MeshDescription>(MeshDescription::JsonName);
	mDescriptionFactory->RegisterDescription<PhysicsDescription>(PhysicsDescription::JsonName);
	mDescriptionFactory->RegisterDescription<PlayerDescription>(PlayerDescription::JsonName);
	mDescriptionFactory->RegisterDescription<SpriteDescription>(SpriteDescription::JsonName);
	mDescriptionFactory->RegisterDescription<TransformDescription>(TransformDescription::JsonName);
}

void EngineContext::_FixedUpdate(float dt) const
{
	mPhysicsSystem->Update(dt, *mEntityRegistry);
}

EngineContext::EngineContext(GameInterface* game, const LLGL::Extent2D screenSize, const LLGL::UTF8String& title,
                             const LLGL::ColorRGBAf backgroundClearColor, bool usePerspective)
{
	_Init(screenSize, title, backgroundClearColor, usePerspective);
	game->Init(this);
	game->RegisterEntityDescriptions();
}

void EngineContext::Run(GameInterface* game) const
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
			game->Update(mTimer->mDT);
			mTimer->mAccumulator -= deltaTime;
		}

		// TODO: Debug draw axis
		//_DrawAxis();

		mRenderContext->BeginFrame();

		//GUISystem::GUIStartFrame();
		//GUISystem::RenderMenus();
		//GUISystem::GUIEndFrame();

		game->Render();
		if (const auto level = mLevelManager->GetCurrentLevel())
		{
			mRenderContext->Render(level->GetCamera(), *mEntityRegistry, *mMapRegistry);
		}

		mRenderContext->EndFrame();

		if (mLuaSystem->QueueCloseScripts())
		{
			mLuaSystem->CloseAllScripts();
		}
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

void EngineContext::FlushEntities() const
{
	mEntityRegistry->FlushEntities();
}

const std::vector<const char*>& EngineContext::GetLevelNames() const
{
	return mLevelManager->GetLevelNames();
}

void EngineContext::LoadLevel(const char* levelName) const
{
	mLevelManager->LoadLevel(levelName, *this, *mMapRegistry, *mLuaSystem);
}

const Level* EngineContext::GetLevel() const
{
	return mLevelManager->GetCurrentLevel();
}

void EngineContext::GenerateMapTexture(EntityId mapId) const
{
	mRenderContext->GenerateMapTexture(*mEntityRegistry, mapId);
}

void EngineContext::DrawPoint(glm::vec3 position, float size, glm::vec4 color) const
{
	mRenderContext->DrawPoint(position, color, size);
}

void EngineContext::DrawLine(glm::vec3 from, glm::vec3 to, glm::vec4 color) const
{
	mRenderContext->DrawLine(from, to, color);
}

void EngineContext::DrawBox(glm::vec3 position, glm::vec3 size, glm::vec4 color, bool filled) const
{
	mRenderContext->DrawBox(position, size, color, filled);
}

void EngineContext::DrawCircle(glm::vec3 position, float radius, glm::vec4 color) const
{
	mRenderContext->DrawCircle(position, radius, color);
}

void EngineContext::DrawGrid(glm::vec3 position, glm::vec3 size, int rows, int columns, glm::vec4 color) const
{
	mRenderContext->DrawGrid(position, size, color, rows, columns);
}

void EngineContext::DrawText2D(const char* text, glm::vec2 position, glm::vec2 size, glm::vec4 color) const
{
	mRenderContext->DrawText(text, position, size, color);
}

void EngineContext::DrawText3D(const char* text, glm::vec3 position, glm::vec3 size) const
{
	// TODO: This will just render 2D text for now
	mRenderContext->DrawText(text, position, size, {1.0f, 1.0f, 1.0f, 1.0f});
}

void EngineContext::SetBackgroundClearColor(const LLGL::ColorRGBAf color) const
{
	mRenderContext->SetBackgroundClearColor(color);
}

void EngineContext::ToggleEditorMode(bool toggle) const
{
	if (toggle)
	{
		if (const auto level = mLevelManager->GetCurrentLevel())
		{
			mInputHandler->AddEditorInputs(level->GetCamera());
		}
	}
}
