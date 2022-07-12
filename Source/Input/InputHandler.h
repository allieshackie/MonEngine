#pragma once

#include <LLGL/LLGL.h>
#include <queue>

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
	InputEvent(LLGL::Key keyCode, KeyStates keyState): mKeyCode(keyCode), mKeyState(keyState) {}
	InputEvent(wchar_t newChar): mKeyState(KeyStates::Char), mChar(newChar) {}
	LLGL::Key mKeyCode = LLGL::Key::Any;
	KeyStates mKeyState = KeyStates::Key_Neutral;
	wchar_t mChar = '_';
};

class InputQueue : public LLGL::Input
{
public:
	InputQueue() = default;

	const InputEvent& ProcessNextEvent()
	{
		return mEventQueue.front();
	}

	void PopEvent()
	{
		mEventQueue.pop();
	}

	bool HasEventsQueued() const
	{
		return !mEventQueue.empty();
	}

protected:
	void OnKeyUp(LLGL::Window& sender, LLGL::Key keyCode) override
	{
		mEventQueue.push({ keyCode, KeyStates::Key_Up });
	}
	void OnKeyDown(LLGL::Window& sender, LLGL::Key keyCode) override
	{
		mEventQueue.push({ keyCode, KeyStates::Key_Down });
	}

	void OnChar(LLGL::Window& sender, wchar_t chr) override
	{
		mEventQueue.push({ chr });
	}

private:
	std::queue<InputEvent> mEventQueue;
};

class InputHandler {
public:
	InputHandler(Window& window);

	void pollInputEvents();
	void pollGUIInputEvents(const std::function<void(const InputEvent & event)>& keyboardCallback) const;

	void registerButtonUpHandler(LLGL::Key keyCode, const std::function<void()>& callback);
	void registerButtonDownHandler(LLGL::Key keyCode, const std::function<void()>& callback);

	void registerMouseMoveHandler(const std::function<void(LLGL::Offset2D)>& callback);
private:
	void _handleButtonUpEvent(LLGL::Key keyCode);
	void _handleButtonDownEvent(LLGL::Key keyCode);

	std::unordered_map<LLGL::Key, std::function<void()>> mButtonUpHandlers;
	std::unordered_map<LLGL::Key, std::function<void()>> mButtonDownHandlers;
	std::function<void(LLGL::Offset2D)> mMouseMoveCallback;
	std::shared_ptr<InputQueue> mInput; // User input event listener

	Window& mWindow;
};