#pragma once

class GUISystem;
class MainGameGUI;
class MapEditor;
class Renderer;

class PipelineGUI
{
public:
	PipelineGUI(Renderer& renderer, GUISystem& guiSystem, MapEditor& mapEditor, MainGameGUI& mainGameGUI);

	~PipelineGUI()
	{
		std::cout << "Delete PipelineGUI" << std::endl;
	}

	void Tick() const;

private:
	GUISystem& mGUISystem;
	MainGameGUI& mMainGameGUI;
	MapEditor& mMapEditor;
	Renderer& mRenderer;
};
