#pragma once

#include <queue>
#include "LLGL/Window.h"

class Window;

enum class KeyStates
{
	Key_Neutral = 0,
	Key_Down,
	Key_Up,
	Key_Hold,
	Char
};

struct InputEvent
{
	InputEvent() = default;

	InputEvent(LLGL::Key keyCode, KeyStates keyState): mKeyCode(keyCode), mKeyState(keyState)
	{
	}

	InputEvent(wchar_t newChar): mKeyState(KeyStates::Char), mChar(newChar)
	{
	}

	LLGL::Key mKeyCode = LLGL::Key::Any;
	KeyStates mKeyState = KeyStates::Key_Neutral;
	wchar_t mChar = '_';
};

class InputHandler : public LLGL::Window::EventListener
{
public:
	friend class InputManager;

protected:
	void OnKeyDown(LLGL::Window& sender, LLGL::Key keyCode) override;
	void OnKeyUp(LLGL::Window& sender, LLGL::Key keyCode) override;
	void OnWheelMotion(LLGL::Window& sender, int motion) override;
	void OnLocalMotion(LLGL::Window& sender, const LLGL::Offset2D& position) override;
	// Global motion is mouse direction, not screen position
	//void OnGlobalMotion(LLGL::Window& sender, const LLGL::Offset2D& position) override;
	void OnChar(LLGL::Window& sender, wchar_t chr) override;

private:
	void _handleKeyDown(LLGL::Key keyCode);
	void _handleKeyUp(LLGL::Key keyCode);

	void _handleKeyDownGUI(LLGL::Key keyCode);
	void _handleKeyUpGUI(LLGL::Key keyCode);

	std::unordered_map<LLGL::Key, std::vector<std::function<void()>>> mButtonUpHandlers;
	std::unordered_map<LLGL::Key, std::vector<std::function<void()>>> mButtonDownHandlers;
	std::vector<std::function<void(LLGL::Offset2D)>> mMouseMoveCallbacks;
	std::vector<std::function<void(InputEvent)>> mMouseScrollCallbacks;
	std::function<void()> mZoomInCallback;
	std::function<void()> mZoomOutCallback;

	LLGL::Offset2D mCurrentMousePos = {0, 0};
};
