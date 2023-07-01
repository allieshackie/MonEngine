#include "GUI/GUIBase.h"
#include "GUI/GUISystem.h"

#include "GUIPipeline.h"

PipelineGUI::PipelineGUI(GUISystem& guiSystem, GUIBase& gui)
	: mGUISystem(guiSystem), mGUI(gui)
{
}

void PipelineGUI::Tick() const
{
	// set the render context as the initial render target
	// Render GUI
	mGUISystem.GUIStartFrame();
	mGUI.RenderGUI();
	mGUISystem.GUIEndFrame();
}
