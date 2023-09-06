#pragma once

#include "Core/ConfigManager.h"
#include "Core/EventListener.h"
#include "Core/LevelManager.h"
#include "Core/Timer.h"
#include "Entity/EntityRegistry.h"
#include "Entity/Descriptions/DescriptionFactory.h"
#include "Graphics/Core/RenderContext.h"
#include "Graphics/Core/ResourceManager.h"
#include "GUI/EditorGUI.h"
#include "Input/InputHandler.h"
#include "Input/InputManager.h"
#include "Map/MapSystem.h"
#include "Physics/CollisionSystem.h"
#include "Physics/PhysicsSystem.h"

class EngineContext
{
public:
	EngineContext() = default;
	virtual ~EngineContext() = default;

	void Init(const char* gameDataFolder);

	void Run() const;

	virtual void Update(float dt) const = 0;
	virtual void RegisterEntityDescriptions() const = 0;

	// TODO:
	// LoadFont
	// CreateGameObject
	// LoadLevel
	// GetLevel
	// OpenMap
	// DrawPoint
	// DrawBox
	// DrawGrid
	// DrawText
	// TODO: 

protected:
	template <typename T>
	void RegisterDescription(const std::string& descriptionName) const
	{
		mDescriptionFactory->RegisterDescription<T>(descriptionName);
	}

private:
	void _InitDescriptions() const;
	void _FixedUpdate(float dt) const;

	std::unique_ptr<DescriptionFactory> mDescriptionFactory = nullptr;
	std::unique_ptr<ConfigManager> mConfigManager = nullptr;
	std::unique_ptr<RenderContext> mRenderContext = nullptr;
	std::unique_ptr<Timer> mTimer = nullptr;

	std::shared_ptr<InputHandler> mInputHandler = nullptr;
	std::unique_ptr<InputManager> mInputManager = nullptr;

	std::unique_ptr<ResourceManager> mResourceManager = nullptr;

	std::unique_ptr<LevelManager> mLevelManager = nullptr;
	std::unique_ptr<EditorGUI> mEditorGUI = nullptr;

	std::unique_ptr<EntityRegistry> mEntityRegistry = nullptr;
	std::unique_ptr<EventPublisher> mEventPublisher = nullptr;

	// systems
	std::unique_ptr<CollisionSystem> mCollisionSystem = nullptr;
	std::unique_ptr<MapSystem> mMapSystem = nullptr;
	std::unique_ptr<PhysicsSystem> mPhysicsSystem = nullptr;

	bool mRunning = true;
};
