#pragma once
#include "MapEditor.h"
#include "Core/ISystem.h"
#include "ContextMenus/EntityMenu.h"
#include "ObjectGUI.h"

class Camera;
class InputHandler;
class WindowContext;
class ResourceManager;
class World;

class EditorGUI : public ISystem
{
public:
	EditorGUI(std::weak_ptr<InputHandler> inputHandler, std::weak_ptr<World> world, WindowContext& windowContext,
	          ResourceManager& resourceManager, RenderSystem& renderSystem);

	void RenderGUI(float dt) override;

	void RenderAxis();

private:
	void _RenderCameraWindow();
	void _RenderInputWindow();

	void _RenderPropertiesWindow();

	std::unique_ptr<MapEditor> mMapEditor;
	std::unique_ptr<ObjectGUI> mObjectGUI;
	std::unique_ptr<EntityMenu> mEntityMenu;

	bool mOpen = true;
	ImVec2 mSize = {250, 780};
	ImGuiWindowFlags mWindowFlags = ImGuiWindowFlags_None;

	std::weak_ptr<InputHandler> mInputHandler;
	std::weak_ptr<World> mWorld;
	ResourceManager& mResourceManager;
	WindowContext& mWindowContext;
	RenderSystem& mRenderSystem;

	int mAxisElement = -1;

	bool showCameraWindow = false;
	bool showInputWindow = false;
	bool showEntityWindow = false;
	bool showAxis = false;
	bool showBonesOutline = false;
};
