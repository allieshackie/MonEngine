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

	const InputEvent& ProcessNextEvent() const
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

	static void registerButtonUpHandler(LLGL::Key keyCode, const std::function<void()>& callback);
	static void registerButtonDownHandler(LLGL::Key keyCode, const std::function<void()>& callback);

	static void registerMouseMoveHandler(const std::function<void(LLGL::Offset2D)>& callback);
	static void registerZoomInHandler(const std::function<void()>& callback);
	static void registerZoomOutHandler(const std::function<void()>& callback);
private:
	void _handleButtonUpEvent(LLGL::Key keyCode);
	void _handleButtonDownEvent(LLGL::Key keyCode);
	void _handleMouseMoveEvent();

	static std::unordered_map<LLGL::Key, std::vector<std::function<void()>>> mButtonUpHandlers;
	static std::unordered_map<LLGL::Key, std::vector<std::function<void()>>> mButtonDownHandlers;
	static std::vector<std::function<void(LLGL::Offset2D)>> mMouseMoveCallbacks;
	static std::function<void()> mZoomInCallback;
	static std::function<void()> mZoomOutCallback;
	std::shared_ptr<InputQueue> mInput; // User input event listener

	LLGL::Offset2D mCurrentMousePos = {0,0};  
};