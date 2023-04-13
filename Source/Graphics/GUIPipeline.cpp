#include "Core/Renderer.h"
#include "GUISystem.h"
#include "Defines.h"
#include "MainGameGUI.h"
#include "MapEditor.h"

#include "GUIPipeline.h"

PipelineGUI::PipelineGUI(Renderer& renderer, GUISystem& guiSystem, MapEditor& mapEditor, MainGameGUI& mainGameGUI)
	: mGUISystem(guiSystem), mMainGameGUI(mainGameGUI), mMapEditor(mapEditor), mRenderer(renderer)
{
}

void PipelineGUI::Tick() const
{
	// set the render context as the initial render target
	// Render GUI
	mGUISystem.GUIStartFrame();
	if (!MonDev::EDIT_MODE)
	{
		mMainGameGUI.RenderGUI();
	}
	if (MonDev::EDIT_MODE)
	{
		mMapEditor.RenderGUI();
	}
	mGUISystem.GUIEndFrame();
}
