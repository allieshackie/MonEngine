#pragma once

#include "Core/EventListener.h"
#include "Core/SceneManager.h"
#include "Core/SystemManager.h"
#include "Editor/EditorGUI.h"
#include "Entity/Descriptions/DescriptionFactory.h"
#include "Graphics/WindowContext.h"
#include "Graphics/Core/ResourceManager.h"
#include "Input/InputContext.h"
#include "Input/InputHandler.h"
#include "Terrain/TerrainSystem.h"

class Game
{
public:
	Game(const LLGL::Extent2D screenSize = {1200, 800},
	        const LLGL::UTF8String& title = "Game",
	        const LLGL::ColorRGBAf backgroundClearColor = {0.0f, 0.0f, 0.0f, 1.0f},
	        bool transparent = false);

	void Run();

	template <typename T>
	void RegisterDescription(const std::string& descriptionName) const
	{
		mDescriptionFactory->RegisterDescription<T>(descriptionName);
	}

	void ToggleEditorMode(bool toggle);

private:
	void InitCoreSystems(); // Init all systems **without** dependencies
	void RegisterDescriptions(); // Must be called before SceneManager sets up description factory
	void InitDependentSystems(const LLGL::Extent2D screenSize, const LLGL::UTF8String& title,
		const LLGL::ColorRGBAf backgroundClearColor, bool transparent);
	void InitGameplaySystems();
	void SetupLuaBindings();

	std::unique_ptr<DescriptionFactory> mDescriptionFactory;
	std::unique_ptr<WindowContext> mWindowContext;
	std::unique_ptr<ResourceManager> mResourceManager;

	std::unique_ptr<SceneManager> mSceneManager;
	std::shared_ptr<GUISystem> mGUISystem;

	std::unique_ptr<EventPublisher> mEventPublisher;
	std::unique_ptr<SystemManager> mSystemManager;
	std::unique_ptr<TerrainSystem> mTerrainSystem;

	// systems
	std::shared_ptr<InputHandler> mInputHandler;
	std::shared_ptr<InputContext> mInputContext;
	std::shared_ptr<RenderSystem> mRenderSystem;

	bool mRunning = true;
};
