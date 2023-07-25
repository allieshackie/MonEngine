#pragma once
#include "GUIBase.h"
#include "MapEditor.h"

class ConfigManager;
class InputManager;
class LevelManager;
class MapSystem;
class Renderer;
class ResourceManager;

class EditorGUI : public GUIBase
{
public:
	EditorGUI(InputManager& inputManager, LevelManager& levelManager, MapSystem& mapSystem, Renderer& renderer,
	          ResourceManager& resourceManager, const ConfigManager& configManager);
	~EditorGUI() override = default;

	void RenderGUI() override;

private:
	void _MainMenu() const;

	std::unique_ptr<MapEditor> mMapEditor;
};
