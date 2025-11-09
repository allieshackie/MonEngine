#pragma once
#include "MapEditor.h"
#include "Core/ISystem.h"
#include "ContextMenus/EntityMenu.h"
#include "ObjectGUI.h"

class Camera;
class InputHandler;
class RenderContext;
class ResourceManager;
class World;

class EditorGUI : public ISystem
{
public:
	EditorGUI(std::weak_ptr<InputHandler> inputHandler, std::weak_ptr<World> world, RenderContext& renderContext,
	          ResourceManager& resourceManager, RenderSystem& renderSystem);

	void RenderGUI() override;

	void RenderAxis();

private:
	std::unique_ptr<MapEditor> mMapEditor;
	std::unique_ptr<ObjectGUI> mObjectGUI;
	std::unique_ptr<EntityMenu> mEntityMenu;

	bool mOpen = true;
	ImVec2 mSize = {250, 780};
	ImGuiWindowFlags mWindowFlags = ImGuiWindowFlags_NoMove;

	std::weak_ptr<InputHandler> mInputHandler;
	std::weak_ptr<World> mWorld;
	ResourceManager& mResourceManager;
	RenderContext& mRenderContext;
	RenderSystem& mRenderSystem;

	int mAxisElement = -1;

	bool showInputSection = true;
	bool showEntitySection = true;
	bool showBonesSection = true;
	bool showAnimationSection = true;
	bool showAxis = true;
	bool showBonesOutline = false;
};
