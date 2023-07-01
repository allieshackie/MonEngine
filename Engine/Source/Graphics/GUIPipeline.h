#pragma once

class GUIBase;
class GUISystem;
class Renderer;

class PipelineGUI
{
public:
	PipelineGUI(GUISystem& guiSystem, GUIBase& gui);

	~PipelineGUI()
	{
		std::cout << "Delete PipelineGUI" << std::endl;
	}

	void Tick() const;

private:
	GUISystem& mGUISystem;
	GUIBase& mGUI;
};
