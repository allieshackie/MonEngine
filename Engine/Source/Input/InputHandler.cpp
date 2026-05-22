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
	_handleKeyUpGUI(keyCode);
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

ImGuiKey InputHandler::LLGLKeyToImGuiKey(LLGL::Key key)
{
	switch (key)
	{
		case LLGL::Key::A: return ImGuiKey_A;
		case LLGL::Key::B: return ImGuiKey_B;
		case LLGL::Key::C: return ImGuiKey_C;
		case LLGL::Key::D: return ImGuiKey_D;
		case LLGL::Key::E: return ImGuiKey_E;
		case LLGL::Key::F: return ImGuiKey_F;
		case LLGL::Key::G: return ImGuiKey_G;
		case LLGL::Key::H: return ImGuiKey_H;
		case LLGL::Key::I: return ImGuiKey_I;
		case LLGL::Key::J: return ImGuiKey_J;
		case LLGL::Key::K: return ImGuiKey_K;
		case LLGL::Key::L: return ImGuiKey_L;
		case LLGL::Key::M: return ImGuiKey_M;
		case LLGL::Key::N: return ImGuiKey_N;
		case LLGL::Key::O: return ImGuiKey_O;
		case LLGL::Key::P: return ImGuiKey_P;
		case LLGL::Key::Q: return ImGuiKey_Q;
		case LLGL::Key::R: return ImGuiKey_R;
		case LLGL::Key::S: return ImGuiKey_S;
		case LLGL::Key::T: return ImGuiKey_T;
		case LLGL::Key::U: return ImGuiKey_U;
		case LLGL::Key::V: return ImGuiKey_V;
		case LLGL::Key::W: return ImGuiKey_W;
		case LLGL::Key::X: return ImGuiKey_X;
		case LLGL::Key::Y: return ImGuiKey_Y;
		case LLGL::Key::Z: return ImGuiKey_Z;

		case LLGL::Key::D0: return ImGuiKey_0;
		case LLGL::Key::D1: return ImGuiKey_1;
		case LLGL::Key::D2: return ImGuiKey_2;
		case LLGL::Key::D3: return ImGuiKey_3;
		case LLGL::Key::D4: return ImGuiKey_4;
		case LLGL::Key::D5: return ImGuiKey_5;
		case LLGL::Key::D6: return ImGuiKey_6;
		case LLGL::Key::D7: return ImGuiKey_7;
		case LLGL::Key::D8: return ImGuiKey_8;
		case LLGL::Key::D9: return ImGuiKey_9;

		case LLGL::Key::F1:  return ImGuiKey_F1;
		case LLGL::Key::F2:  return ImGuiKey_F2;
		case LLGL::Key::F3:  return ImGuiKey_F3;
		case LLGL::Key::F4:  return ImGuiKey_F4;
		case LLGL::Key::F5:  return ImGuiKey_F5;
		case LLGL::Key::F6:  return ImGuiKey_F6;
		case LLGL::Key::F7:  return ImGuiKey_F7;
		case LLGL::Key::F8:  return ImGuiKey_F8;
		case LLGL::Key::F9:  return ImGuiKey_F9;
		case LLGL::Key::F10: return ImGuiKey_F10;
		case LLGL::Key::F11: return ImGuiKey_F11;
		case LLGL::Key::F12: return ImGuiKey_F12;

		case LLGL::Key::Return:    return ImGuiKey_Enter;
		case LLGL::Key::Escape:    return ImGuiKey_Escape;
		case LLGL::Key::Back:	   return ImGuiKey_Backspace;
		case LLGL::Key::Tab:       return ImGuiKey_Tab;
		case LLGL::Key::Space:     return ImGuiKey_Space;
		case LLGL::Key::Delete:    return ImGuiKey_Delete;
		case LLGL::Key::Insert:    return ImGuiKey_Insert;
		case LLGL::Key::Home:      return ImGuiKey_Home;
		case LLGL::Key::End:       return ImGuiKey_End;
		case LLGL::Key::PageUp:    return ImGuiKey_PageUp;
		case LLGL::Key::PageDown:  return ImGuiKey_PageDown;

		case LLGL::Key::Left:  return ImGuiKey_LeftArrow;
		case LLGL::Key::Right: return ImGuiKey_RightArrow;
		case LLGL::Key::Up:    return ImGuiKey_UpArrow;
		case LLGL::Key::Down:  return ImGuiKey_DownArrow;

		case LLGL::Key::LShift:   return ImGuiKey_LeftShift;
		case LLGL::Key::RShift:   return ImGuiKey_RightShift;
		case LLGL::Key::LControl: return ImGuiKey_LeftCtrl;
		case LLGL::Key::RControl: return ImGuiKey_RightCtrl;

		case LLGL::Key::Keypad0: return ImGuiKey_Keypad0;
		case LLGL::Key::Keypad1: return ImGuiKey_Keypad1;
		case LLGL::Key::Keypad2: return ImGuiKey_Keypad2;
		case LLGL::Key::Keypad3: return ImGuiKey_Keypad3;
		case LLGL::Key::Keypad4: return ImGuiKey_Keypad4;
		case LLGL::Key::Keypad5: return ImGuiKey_Keypad5;
		case LLGL::Key::Keypad6: return ImGuiKey_Keypad6;
		case LLGL::Key::Keypad7: return ImGuiKey_Keypad7;
		case LLGL::Key::Keypad8: return ImGuiKey_Keypad8;
		case LLGL::Key::Keypad9: return ImGuiKey_Keypad9;
		case LLGL::Key::KeypadPlus:     return ImGuiKey_KeypadAdd;
		case LLGL::Key::KeypadMinus:    return ImGuiKey_KeypadSubtract;
		case LLGL::Key::KeypadMultiply: return ImGuiKey_KeypadMultiply;
		case LLGL::Key::KeypadDivide:   return ImGuiKey_KeypadDivide;
		case LLGL::Key::KeypadDecimal:  return ImGuiKey_KeypadDecimal;

		default: return ImGuiKey_None;
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
			io.AddKeyEvent(LLGLKeyToImGuiKey(keyCode), true);
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
			io.AddKeyEvent(LLGLKeyToImGuiKey(keyCode), false);
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
