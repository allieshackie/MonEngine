#pragma once

#include "Core/EventListener.h"
#include "Core/SceneManager.h"
#include "Core/SystemManager.h"
#include "Editor/EditorGUI.h"
#include "Entity/Descriptions/DescriptionFactory.h"
#include "Graphics/RenderContext.h"
#include "Graphics/Core/ResourceManager.h"
#include "Input/InputHandler.h"
#include "Map/MapRegistry.h"
#include "Script/LuaSystem.h"

class Sandbox
{
public:
	Sandbox(const LLGL::Extent2D screenSize = {1200, 800},
	        const LLGL::UTF8String& title = "Sandbox",
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
	std::unique_ptr<DescriptionFactory> mDescriptionFactory;
	std::unique_ptr<RenderContext> mRenderContext;
	std::unique_ptr<ResourceManager> mResourceManager;

	std::unique_ptr<SceneManager> mSceneManager;
	std::unique_ptr<LuaSystem> mLuaSystem;
	std::shared_ptr<GUISystem> mGUISystem;

	std::unique_ptr<EventPublisher> mEventPublisher;
	std::unique_ptr<SystemManager> mSystemManager;
	std::unique_ptr<MapRegistry> mMapRegistry;

	// systems
	std::shared_ptr<InputHandler> mInputHandler;
	RenderSystem* mRenderSystem;

	bool mRunning = true;
};
