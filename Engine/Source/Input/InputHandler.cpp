#include "imgui.h"
#include "Core/Camera.h"
#include "GUI/GUISystem.h"

#include "InputHandler.h"

void InputHandler::RegisterButtonUpHandler(LLGL::Key keyCode, const std::function<void()>& callback)
{
	const auto& handler = mButtonUpHandlers.find(keyCode);
	if (handler != mButtonUpHandlers.end())
	{
		handler->second.push_back({callback});
	}
	else
	{
		mButtonUpHandlers.insert({keyCode, {callback}});
	}
}

void InputHandler::RegisterButtonDownHandler(LLGL::Key keyCode, const std::function<void()>& callback)
{
	const auto& handler = mButtonDownHandlers.find(keyCode);
	if (handler != mButtonDownHandlers.end())
	{
		handler->second.push_back({callback});
	}
	else
	{
		mButtonDownHandlers.insert({keyCode, {callback}});
	}
}

void InputHandler::RegisterMouseMoveHandler(const std::function<void(LLGL::Offset2D)>& callback)
{
	mMouseMoveCallbacks.push_back(callback);
}

void InputHandler::RegisterZoomInHandler(const std::function<void()>& callback)
{
	mZoomInCallback = callback;
}

void InputHandler::RegisterZoomOutHandler(const std::function<void()>& callback)
{
	mZoomOutCallback = callback;
}

void InputHandler::AddEditorInputs(Camera& camera)
{
	// Register camera handlers for moving the camera position
	// If the mCameraFront remains the same, this will result in the
	// camera view angling.  We can adjust the mCameraFront.xy to match the
	// camera position so that the view will not angle
	RegisterButtonUpHandler(LLGL::Key::Up, [&camera]() { camera.MoveUp(); });
	RegisterButtonUpHandler(LLGL::Key::Down, [&camera]() { camera.MoveDown(); });
	RegisterButtonUpHandler(LLGL::Key::Left, [&camera]() { camera.MoveLeft(); });
	RegisterButtonUpHandler(LLGL::Key::Right, [&camera]() { camera.MoveRight(); });

	// Handlers for handling the camera zoom
	RegisterZoomInHandler([&camera]() { camera.ZoomIn(); });
	RegisterZoomOutHandler([&camera]() { camera.ZoomOut(); });
}

void InputHandler::OnKeyDown(LLGL::Window& sender, LLGL::Key keyCode)
{
	if (GUISystem::IsGUIContext())
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
	if (GUISystem::IsGUIContext())
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
	if (GUISystem::IsGUIContext())
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
	if (GUISystem::IsGUIContext())
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
