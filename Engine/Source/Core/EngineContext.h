#pragma once

#include "Core/EventListener.h"
#include "Core/LevelManager.h"
#include "Core/Timer.h"
#include "Entity/EntityRegistry.h"
#include "Entity/Descriptions/DescriptionFactory.h"
#include "Graphics/RenderContext.h"
#include "Graphics/Core/ResourceManager.h"
#include "Input/InputHandler.h"
#include "Map/MapRegistry.h"
#include "Physics/CollisionSystem.h"
#include "Physics/PhysicsSystem.h"

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

	void SetGUIMenu(std::unique_ptr<GUIBase> gui);

	void Run() const;

	virtual void Update(float dt) const = 0;
	virtual void RegisterEntityDescriptions() const = 0;

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

protected:
	void Init();

	template <typename T>
	void RegisterDescription(const std::string& descriptionName) const
	{
		mDescriptionFactory->RegisterDescription<T>(descriptionName);
	}

private:
	void _InitDescriptions() const;
	void _FixedUpdate(float dt) const;

	void _DrawAxis() const;

	std::unique_ptr<DescriptionFactory> mDescriptionFactory = nullptr;
	std::unique_ptr<RenderContext> mRenderContext = nullptr;
	std::unique_ptr<Timer> mTimer = nullptr;

	std::shared_ptr<InputHandler> mInputHandler = nullptr;

	std::unique_ptr<ResourceManager> mResourceManager = nullptr;

	std::unique_ptr<LevelManager> mLevelManager = nullptr;
	std::unique_ptr<GUIBase> mGUIMenu = nullptr;

	std::unique_ptr<EntityRegistry> mEntityRegistry = nullptr;
	std::unique_ptr<EventPublisher> mEventPublisher = nullptr;

	// systems
	std::unique_ptr<CollisionSystem> mCollisionSystem = nullptr;
	std::unique_ptr<MapRegistry> mMapRegistry = nullptr;
	std::unique_ptr<PhysicsSystem> mPhysicsSystem = nullptr;

	bool mRunning = true;

	bool mDebugDraw = false;
};

template <typename Component>
inline Component& EngineContext::GetComponent(EntityId id) const
{
	return mEntityRegistry->GetComponent<Component>(id);
}
