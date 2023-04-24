#include "imgui/imgui.h"
#include "GUISystem.h"

#include "InputHandler.h"

InputHandler::InputHandler(GUISystem& guiSystem) : mGUISystem(guiSystem)
{
}

void InputHandler::OnKeyDown(LLGL::Window& sender, LLGL::Key keyCode)
{
	if (mGUISystem.isGUIContext())
	{
		_handleKeyDownGUI(keyCode);
	}
	else
	{
		_handleKeyDown(keyCode);
	}
}

void InputHandler::OnKeyUp(LLGL::Window& sender, LLGL::Key keyCode)
{
	if (mGUISystem.isGUIContext())
	{
		_handleKeyUpGUI(keyCode);
	}
	else
	{
		_handleKeyUp(keyCode);
	}
}

void InputHandler::OnWheelMotion(LLGL::Window& sender, int motion)
{
	if (mGUISystem.isGUIContext())
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
			if (mZoomOutCallback)
			{
				mZoomOutCallback();
			}
		}
		else
		{
			if (mZoomInCallback)
			{
				mZoomInCallback();
			}
		}
	}
}

void InputHandler::OnLocalMotion(LLGL::Window& sender, const LLGL::Offset2D& position)
{
	for (const auto& fn : mMouseMoveCallbacks)
	{
		fn(position);
	}
}

void InputHandler::OnChar(LLGL::Window& sender, wchar_t chr)
{
	if (mGUISystem.isGUIContext())
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddInputCharacterUTF16(chr);
	}
}

void InputHandler::_handleKeyDown(LLGL::Key keyCode)
{
	const auto& handler = mButtonDownHandlers.find(keyCode);
	if (handler != mButtonDownHandlers.end())
	{
		for (const auto& cb : handler->second)
		{
			cb();
		}
	}
}

void InputHandler::_handleKeyUp(LLGL::Key keyCode)
{
	const auto& handler = mButtonUpHandlers.find(keyCode);
	if (handler != mButtonUpHandlers.end())
	{
		for (const auto& cb : handler->second)
		{
			cb();
		}
	}
}

void InputHandler::_handleKeyDownGUI(LLGL::Key keyCode)
{
	ImGuiIO& io = ImGui::GetIO();
	switch (keyCode)
	{
	// Mouse buttons: 0=left, 1=right, 2=middle
	case LLGL::Key::LButton:
		{
			io.MouseDown[0] = true;
			break;
		}
	// Mouse buttons: 0=left, 1=right, 2=middle
	case LLGL::Key::RButton:
		{
			io.MouseDown[1] = true;
			break;
		}
	default:
		{
			if (static_cast<int>(keyCode) < 256)
			{
				io.KeysDown[static_cast<int>(keyCode)] = true;
			}
			break;
		}
	}
}

void InputHandler::_handleKeyUpGUI(LLGL::Key keyCode)
{
	ImGuiIO& io = ImGui::GetIO();
	switch (keyCode)
	{
	// Mouse buttons: 0=left, 1=right, 2=middle
	case LLGL::Key::LButton:
		{
			io.MouseDown[0] = false;
			break;
		}
	// Mouse buttons: 0=left, 1=right, 2=middle
	case LLGL::Key::RButton:
		{
			io.MouseDown[1] = false;
			break;
		}
	default:
		{
			if (static_cast<int>(keyCode) < 256)
			{
				io.KeysDown[static_cast<int>(keyCode)] = false;
			}
			break;
		}
	}
}
