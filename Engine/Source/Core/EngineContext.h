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

class GameInterface;

class EngineContext
{
public:
	EngineContext() = default;
	virtual ~EngineContext() = default;

	// Since we define a destructor, rule of 5 states that we should define
	// all copy and move constructors/operators
	// Deleting to simplify ownership
	EngineContext(EngineContext& other) = delete;
	EngineContext& operator=(EngineContext& other) = delete;
	EngineContext(EngineContext&& other) = delete;
	EngineContext& operator=(EngineContext&& other) = delete;

	void Run(GameInterface* game);

	void SetGUIMenu(std::unique_ptr<GUIBase> gui);
	void LoadFont(const char* fontFileName) const;

	EntityId CreateGameObject(const std::string& entityTemplateName) const;

	template <typename Component>
	Component& GetComponent(EntityId id) const;

	void LoadLevel(const char* levelName) const;
	const std::unique_ptr<Level>& GetLevel() const;

	void OpenMap(const char* mapName) const;
	void OpenMap(const char* mapName, glm::vec3 position, glm::vec3 rotation, float tileSize) const;

	static void DrawPoint(glm::vec3 position, float size);
	static void DrawBox(glm::vec3 position, glm::vec3 size);
	static void DrawGrid(glm::vec3 position, glm::vec3 size, int rows, int columns);
	void DrawText2D(const char* text, glm::vec2 position, glm::vec2 size);
	void DrawText3D(const char* text, glm::vec3 position, glm::vec3 size);

	template <typename T>
	void RegisterDescription(const std::string& descriptionName) const
	{
		mDescriptionFactory->RegisterDescription<T>(descriptionName);
	}

private:
	void _Init();
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
