#pragma once

#include "Core/EventListener.h"
#include "Core/SceneManager.h"
#include "Editor/EditorGUI.h"
#include "Entity/Descriptions/DescriptionFactory.h"
#include "Entity/Systems/InteractSystem.h"
#include "Graphics/RenderContext.h"
#include "Graphics/Core/ResourceManager.h"
#include "GUI/GUIBase.h"
#include "Input/InputHandler.h"
#include "Map/MapRegistry.h"
#include "Physics/PhysicsSystem.h"
#include "Scripting/LuaSystem.h"

struct ModelComponent;

class App
{
public:
	App(const LLGL::Extent2D screenSize = {800, 600},
	    const LLGL::UTF8String& title = "App",
	    const LLGL::ColorRGBAf backgroundClearColor = {0.0f, 0.0f, 0.0f, 1.0f},
	    bool usePerspective = true, bool transparent = false);

	void Run() const;

	void SetGUIMenu(std::unique_ptr<GUIBase> gui);

	template <typename T>
	void RegisterDescription(const std::string& descriptionName) const
	{
		mDescriptionFactory->RegisterDescription<T>(descriptionName);
	}

	void ToggleEditorMode(bool toggle) const;

private:
	void _FixedUpdate(float dt) const;
	void _DebugDrawBones() const;
	void _RenderModelBones(Model& model, const ModelComponent& modelComp, int nodeIndex,
	                       const glm::mat4 parentTransform) const;

	std::unique_ptr<DescriptionFactory> mDescriptionFactory;
	std::unique_ptr<RenderContext> mRenderContext;

	std::shared_ptr<InputHandler> mInputHandler;

	std::unique_ptr<ResourceManager> mResourceManager;
	std::unique_ptr<Animator> mAnimator;

	std::unique_ptr<SceneManager> mSceneManager;
	std::unique_ptr<GUIBase> mGUIMenu;
	std::unique_ptr<LuaSystem> mLuaSystem;
	std::unique_ptr<EditorGUI> mEditorGUI;

	std::unique_ptr<EventPublisher> mEventPublisher;

	// systems
	std::unique_ptr<InteractSystem> mInteractSystem;
	std::unique_ptr<MapRegistry> mMapRegistry;
	std::unique_ptr<PhysicsSystem> mPhysicsSystem;

	bool mRunning = true;
	bool mDebugDraw = false;
};
