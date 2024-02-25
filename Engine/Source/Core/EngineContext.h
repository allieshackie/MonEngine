#pragma once

#include "Core/EventListener.h"
#include "Core/LevelManager.h"
#include "Core/Timer.h"
#include "Entity/EntityRegistry.h"
#include "Entity/Descriptions/DescriptionFactory.h"
#include "Graphics/RenderContext.h"
#include "Graphics/Core/ResourceManager.h"
#include "GUI/GUIBase.h"
#include "Input/InputHandler.h"
#include "Map/MapRegistry.h"
#include "Physics/CollisionSystem.h"
#include "Physics/PhysicsSystem.h"
#include "Scripting/LuaSystem.h"

class GameInterface;

class EngineContext
{
public:
	EngineContext(GameInterface* game, const LLGL::Extent2D screenSize = {800, 600},
	              const LLGL::UTF8String& title = "MonDev",
	              const LLGL::ColorRGBAf backgroundClearColor = {0.0f, 0.0f, 0.0f, 1.0f}, bool usePerspective = true);
	virtual ~EngineContext() = default;

	// Since we define a destructor, rule of 5 states that we should define
	// all copy and move constructors/operators
	// Deleting to simplify ownership
	EngineContext(EngineContext& other) = delete;
	EngineContext& operator=(EngineContext& other) = delete;
	EngineContext(EngineContext&& other) = delete;
	EngineContext& operator=(EngineContext&& other) = delete;

	void Run(GameInterface* game) const;

	void SetGUIMenu(std::unique_ptr<GUIBase> gui);
	void LoadFont(const char* fontFileName) const;

	EntityRegistry& GetEntityRegistry() const;
	InputHandler& GetInputHandler() const;

	EntityId CreateGameObject(const std::string& entityTemplateName) const;
	template <typename Component>
	Component& GetComponent(EntityId id) const;
	void FlushEntities() const;

	const std::vector<const char*>& GetLevelNames() const;
	void LoadLevel(const char* levelName) const;
	const Level* GetLevel() const;

	void OpenMap(const char* mapName) const;
	void InitMapRendering(Map& map) const;

	void DrawPoint(glm::vec3 position, float size, glm::vec4 color) const;
	void DrawLine(glm::vec3 from, glm::vec3 to, glm::vec4 color) const;
	void DrawBox(glm::vec3 position, glm::vec3 size, glm::vec4 color, bool filled = true) const;
	void DrawCircle(glm::vec3 position, float radius, glm::vec4 color) const;
	void DrawGrid(glm::vec3 position, glm::vec3 size, int rows, int columns, glm::vec4 color) const;
	void DrawText2D(const char* text, glm::vec2 position, glm::vec2 size,
	                glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f}) const;
	void DrawText3D(const char* text, glm::vec3 position, glm::vec3 size) const;

	void SetBackgroundClearColor(const LLGL::ColorRGBAf color) const;

	template <typename T>
	void RegisterDescription(const std::string& descriptionName) const
	{
		mDescriptionFactory->RegisterDescription<T>(descriptionName);
	}

	void ToggleEditorMode(bool toggle) const;

private:
	void _Init(const LLGL::Extent2D screenSize, const LLGL::UTF8String& title,
	           const LLGL::ColorRGBAf backgroundClearColor, bool usePerspective);
	void _InitDescriptions() const;

	void _FixedUpdate(float dt) const;
	void _DrawAxis() const;

	std::unique_ptr<DescriptionFactory> mDescriptionFactory;
	std::unique_ptr<RenderContext> mRenderContext;
	std::unique_ptr<Timer> mTimer;

	std::shared_ptr<InputHandler> mInputHandler;

	std::unique_ptr<ResourceManager> mResourceManager;

	std::unique_ptr<LevelManager> mLevelManager;
	std::unique_ptr<GUIBase> mGUIMenu;
	std::unique_ptr<LuaSystem> mLuaSystem;

	std::unique_ptr<EntityRegistry> mEntityRegistry;
	std::unique_ptr<EventPublisher> mEventPublisher;

	// systems
	std::unique_ptr<CollisionSystem> mCollisionSystem;
	std::unique_ptr<MapRegistry> mMapRegistry;
	std::unique_ptr<PhysicsSystem> mPhysicsSystem;

	bool mRunning = true;
	bool mDebugDraw = false;
};

template <typename Component>
inline Component& EngineContext::GetComponent(EntityId id) const
{
	return mEntityRegistry->GetComponent<Component>(id);
}
