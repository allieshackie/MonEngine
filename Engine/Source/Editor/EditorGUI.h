#pragma once
#include "GUI/GUIBase.h"
#include "MapEditor.h"

class EngineContext;
class InputHandler;
class LevelManager;
class MapRegistry;
class RenderContext;

class EditorGUI : public GUIBase
{
public:
	EditorGUI(EngineContext& engineContext, InputHandler& inputHandler, LevelManager& levelManager,
	          MapRegistry& mapRegistry, RenderContext& renderContext);

	void RenderGUI() override;

private:
	void _MainMenu(const std::unique_ptr<Camera>& camera) const;

	std::unique_ptr<MapEditor> mMapEditor;
	InputHandler& mInputHandler;
};
