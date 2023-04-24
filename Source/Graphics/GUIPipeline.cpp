#include "GUISystem.h"
#include "MainGameGUI.h"

#include "GUIPipeline.h"

PipelineGUI::PipelineGUI(GUISystem& guiSystem, MainGameGUI& mainGameGUI)
	: mGUISystem(guiSystem), mMainGameGUI(mainGameGUI)
{
}

void PipelineGUI::Tick() const
{
	// set the render context as the initial render target
	// Render GUI
	mGUISystem.GUIStartFrame();
	mMainGameGUI.RenderGUI();
	mGUISystem.GUIEndFrame();
}
