#include <imgui.h>
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

void InputHandler::RegisterButtonHoldHandler(LLGL::Key keyCode, const std::function<void()>& onHold,
                                             const std::function<void()>& onRelease)
{
	mButtonHoldHandlers[keyCode].push_back({onHold, onRelease});
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

const char* InputHandler::GetNameForKey(LLGL::Key key) const
{
	if (const auto it = mKeyNames.find(key); it != mKeyNames.end())
	{
		return it->second;
	}
	return "Unknown";
}

void InputHandler::AddEditorInputs(Camera& camera)
{
	// Register camera handlers for moving the camera position
	// If the mCameraFront remains the same, this will result in the
	// camera view angling.  We can adjust the mCameraFront.xy to match the
	// camera position so that the view will not angle
	RegisterButtonHoldHandler(LLGL::Key::Up, [&camera]() { camera.MoveUp(); }, []()
	{
	});
	RegisterButtonHoldHandler(LLGL::Key::Down, [&camera]() { camera.MoveDown(); }, []()
	{
	});
	RegisterButtonHoldHandler(LLGL::Key::Left, [&camera]() { camera.MoveLeft(); }, []()
	{
	});
	RegisterButtonHoldHandler(LLGL::Key::Right, [&camera]() { camera.MoveRight(); }, []()
	{
	});
	RegisterButtonUpHandler(LLGL::Key::C, [&camera]() { camera.ToggleFollowCam(); });

	// Handlers for handling the camera zoom
	RegisterZoomInHandler([&camera]() { camera.ZoomIn(); });
	RegisterZoomOutHandler([&camera]() { camera.ZoomOut(); });
}

void InputHandler::Update()
{
	for (auto keyCode : mKeysHeld)
	{
		if (mButtonHoldHandlers.find(keyCode) != mButtonHoldHandlers.end())
		{
			for (auto& handler : mButtonHoldHandlers[keyCode])
			{
				handler.onHold(); // Call the hold function while the key is held down
			}
		}
	}
}

void InputHandler::SetGUISystem(std::weak_ptr<GUISystem> system)
{
	mGUISystemWPtr = system;
}

void InputHandler::OnKeyDown(LLGL::Window& sender, LLGL::Key keyCode)
{
	if (const auto guiSystem = mGUISystemWPtr.lock(); guiSystem->IsGUIContext())
	{
		_handleKeyDownGUI(keyCode);
	}
	else
	{
		_handleKeyDown(keyCode);

		if (mButtonHoldHandlers.find(keyCode) != mButtonHoldHandlers.end())
		{
			if (mKeysHeld.find(keyCode) == mKeysHeld.end())
			{
				// First press, invoke the hold function
				for (auto& handler : mButtonHoldHandlers[keyCode])
					handler.onHold();
			}
			// Add key to the set of held keys
			mKeysHeld.insert(keyCode);
		}
	}

	_AddToDebugKeys(keyCode);
}

void InputHandler::OnKeyUp(LLGL::Window& sender, LLGL::Key keyCode)
{
	if (const auto guiSystem = mGUISystemWPtr.lock(); guiSystem->IsGUIContext())
	{
		_handleKeyUpGUI(keyCode);
	}
	else
	{
		_handleKeyUp(keyCode);

		if (mButtonHoldHandlers.find(keyCode) != mButtonHoldHandlers.end() && mKeysHeld.find(keyCode) != mKeysHeld.
			end())
		{
			// Call the release function
			for (auto& handler : mButtonHoldHandlers[keyCode])
			{
				handler.onRelease();
			}

			// Remove the key from the held keys set
			mKeysHeld.erase(keyCode);
		}
	}

	_RemoveFromDebugKeys(keyCode);
}

void InputHandler::OnWheelMotion(LLGL::Window& sender, int motion)
{
	if (const auto guiSystem = mGUISystemWPtr.lock(); guiSystem->IsGUIContext())
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
	if (const auto guiSystem = mGUISystemWPtr.lock(); guiSystem->IsGUIContext())
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
				io.AddKeyEvent(static_cast<ImGuiKey>(keyCode), true);
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
				io.AddKeyEvent(static_cast<ImGuiKey>(keyCode), false);
			}
			break;
		}
	}
}

void InputHandler::_AddToDebugKeys(LLGL::Key keyCode)
{
	bool duplicate = false;
	for (const auto key : mDebugKeysPressed)
	{
		if (key == keyCode)
		{
			duplicate = true;
			break;
		}
	}

	if (!duplicate)
	{
		mDebugKeysPressed.push_back(keyCode);
	}
}

void InputHandler::_RemoveFromDebugKeys(LLGL::Key keyCode)
{
	mPreviousKeysPressed = mDebugKeysPressed;
	for (auto it = mDebugKeysPressed.begin(); it != mDebugKeysPressed.end();)
	{
		if (*it == keyCode)
		{
			it = mDebugKeysPressed.erase(it);
		}
		else
		{
			++it;
		}
	}
}
