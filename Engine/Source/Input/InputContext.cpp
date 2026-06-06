#include <imgui.h>

#include "GUI/GUISystem.h"
#include "LLGL/Window.h"
#include "InputHandler.h"

#include "InputContext.h"

InputContext::InputContext(InputHandler& inputHandler, GUISystem& guiSystem) : mInputHandler(inputHandler), mGUISystem(guiSystem)
{
}

void InputContext::OnKeyDown(LLGL::Window& sender, LLGL::Key keyCode)
{
	if (mGUISystem.IsGUIContext())
	{
		mInputHandler.HandleKeyDownGUI(keyCode);
	}
	else
	{
		mInputHandler.HandleKeyDown(keyCode);
	}

	mInputHandler.AddToDebugKeys(keyCode);
}

void InputContext::OnKeyUp(LLGL::Window& sender, LLGL::Key keyCode)
{
	mInputHandler.HandleKeyUpGUI(keyCode);
	mInputHandler.HandleKeyUp(keyCode);

	mInputHandler.RemoveFromDebugKeys(keyCode);
}

void InputContext::OnWheelMotion(LLGL::Window& sender, int motion)
{
	if (mGUISystem.IsGUIContext())
	{
		ImGuiIO& io = ImGui::GetIO();
		if (motion > 0)
		{
			io.MouseWheel += 1;
		}
		else
		{
			io.MouseWheel += -1;
		}
	}

	else
	{
		if (motion > 0)
		{
			mInputHandler.TriggerZoomOutCallback();
		}
		else
		{
			mInputHandler.TriggerZoomInCallback();
		}
	}
}

void InputContext::OnLocalMotion(LLGL::Window& sender, const LLGL::Offset2D& position)
{
	mInputHandler.TriggerMouseMoveCallbacks(position);
}

void InputContext::OnChar(LLGL::Window& sender, wchar_t chr)
{
	if (mGUISystem.IsGUIContext())
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddInputCharacterUTF16(chr);
	}
}