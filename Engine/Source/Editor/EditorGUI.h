#pragma once
#include "MapEditor.h"
#include "Core/ISystem.h"
#include "ContextMenus/EntityMenu.h"
#include "ObjectGUI.h"

class Camera;
class InputHandler;
class EventPublisher;
class ResourceManager;
class SceneManager;
class WindowContext;
class World;

class EditorGUI : public ISystem
{
public:
	EditorGUI(std::weak_ptr<InputHandler> inputHandler, RenderSystem& renderSystem, ResourceManager& resourceManager,
			SceneManager& sceneManager, WindowContext& windowContext, EventPublisher& eventPublisher);

	void RenderGUI(float dt, float fps) override;

	void RenderAxis();

private:
	void _RenderCameraWindow();
	void _RenderInputWindow();
	void _RenderPropertiesWindow();
	void _RenderFPSWindow(float fps);

	std::unique_ptr<MapEditor> mMapEditor;
	std::unique_ptr<ObjectGUI> mObjectGUI;
	std::unique_ptr<EntityMenu> mEntityMenu;

	bool mOpen = true;
	ImVec2 mSize = {250, 780};
	ImGuiWindowFlags mWindowFlags = ImGuiWindowFlags_None;
	ImGuiWindowFlags mHiddenWindowFlags = ImGuiWindowFlags_NoDecoration | 
										  ImGuiWindowFlags_NoInputs |
									      ImGuiWindowFlags_NoMove |
										  ImGuiWindowFlags_NoSavedSettings |
										  ImGuiWindowFlags_NoBringToFrontOnFocus |
										  ImGuiWindowFlags_NoNav;

	std::weak_ptr<InputHandler> mInputHandler;
	RenderSystem& mRenderSystem;
	ResourceManager& mResourceManager;
	SceneManager& mSceneManager;
	WindowContext& mWindowContext;
	std::weak_ptr<World> mWorld;

	int mAxisElement = -1;

	bool showCameraWindow = false;
	bool showInputWindow = false;
	bool showEntityWindow = false;
	bool showFPS = true;
	bool showAxis = false;
	bool showBonesOutline = false;
};
