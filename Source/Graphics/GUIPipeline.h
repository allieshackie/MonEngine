#pragma once

class GUISystem;
class MainGameGUI;
class Renderer;

class PipelineGUI
{
public:
	PipelineGUI(GUISystem& guiSystem, MainGameGUI& mainGameGUI);

	~PipelineGUI()
	{
		std::cout << "Delete PipelineGUI" << std::endl;
	}

	void Tick() const;

private:
	GUISystem& mGUISystem;
	MainGameGUI& mMainGameGUI;
};
