#pragma once

#include "Core/EventListener.h"
#include "Core/SceneManager.h"
#include "Editor/EditorGUI.h"
#include "Entity/Descriptions/DescriptionFactory.h"
#include "Entity/Systems/MovementSystem.h"
#include "Entity/Systems/PlayerSystem.h"
#include "Graphics/RenderContext.h"
#include "Graphics/Core/ResourceManager.h"
#include "GUI/GUIBase.h"
#include "Input/InputHandler.h"
#include "Map/MapRegistry.h"
#include "Physics/PhysicsSystem.h"
#include "Scripting/LuaSystem.h"

class Sandbox
{
public:
	Sandbox(const LLGL::Extent2D screenSize = {800, 600},
	        const LLGL::UTF8String& title = "Sandbox",
	        const LLGL::ColorRGBAf backgroundClearColor = {0.0f, 0.0f, 0.0f, 1.0f},
	        bool usePerspective = true, bool transparent = false);

	void Run();

	void SetGUIMenu(std::unique_ptr<GUIBase> gui);

	template <typename T>
	void RegisterDescription(const std::string& descriptionName) const
	{
		mDescriptionFactory->RegisterDescription<T>(descriptionName);
	}

	void ToggleEditorMode(bool toggle) const;

private:
	void _FixedUpdate(float dt) const;

	// Debug
	void _DrawAxis() const;
	void _DebugDrawBones() const;
	void _RenderModelBones(Model& model, const ModelComponent& modelComp, int nodeIndex,
	                       const glm::mat4 parentTransform) const;

	std::unique_ptr<DescriptionFactory> mDescriptionFactory;
	std::unique_ptr<RenderContext> mRenderContext;
	std::unique_ptr<RenderSystem> mRenderSystem;

	std::shared_ptr<InputHandler> mInputHandler;

	std::unique_ptr<ResourceManager> mResourceManager;
	std::unique_ptr<AnimatorSystem> mAnimator;

	std::unique_ptr<SceneManager> mSceneManager;
	std::unique_ptr<GUIBase> mGUIMenu;
	std::unique_ptr<LuaSystem> mLuaSystem;
	std::unique_ptr<EditorGUI> mEditorGUI;

	std::unique_ptr<EventPublisher> mEventPublisher;

	// systems
	std::unique_ptr<MapRegistry> mMapRegistry;
	std::unique_ptr<PhysicsSystem> mPhysicsSystem;
	std::unique_ptr<MovementSystem> mMovementSystem;
	std::unique_ptr<PlayerSystem> mPlayerSystem;

	bool mRunning = true;
	bool mDebugDraw = false;
};
