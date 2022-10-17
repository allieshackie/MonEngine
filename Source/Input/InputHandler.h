#pragma once

#include <LLGL/LLGL.h>
#include <queue>
#include <set>

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

class InputQueue : public LLGL::Input
{
public:
	InputQueue() = default;

	~InputQueue() override = default;

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

	const std::set<LLGL::Key>& GetKeyDownArray() const
	{
		return mKeyDown;
	}

	void PushEvent(LLGL::Key keyCode, KeyStates state)
	{
		mEventQueue.push({keyCode, state});
	}

protected:
	void OnKeyUp(LLGL::Window& sender, LLGL::Key keyCode) override
	{
		if (const auto it = mKeyDown.find(keyCode); it != mKeyDown.end())
		{
			mKeyDown.erase(it);
		}

		mEventQueue.push({keyCode, KeyStates::Key_Up});
	}

	void OnKeyDown(LLGL::Window& sender, LLGL::Key keyCode) override
	{
		if (const auto it = mKeyDown.find(keyCode); it == mKeyDown.end())
		{
			mKeyDown.insert(keyCode);
		}
	}

	void OnChar(LLGL::Window& sender, wchar_t chr) override
	{
		mEventQueue.push({chr});
	}

private:
	std::queue<InputEvent> mEventQueue;
	std::set<LLGL::Key> mKeyDown;
};

class InputHandler
{
public:
	InputHandler(Window& window);

	void pollInputEvents();
	void pollGUIInputEvents(const std::function<void(const InputEvent& event)>& keyboardCallback) const;

	friend class InputManager;

private:
	void _handleButtonUpEvent(LLGL::Key keyCode);
	void _handleButtonDownEvent(LLGL::Key keyCode);
	void _handleMouseMoveEvent();

	std::unordered_map<LLGL::Key, std::vector<std::function<void()>>> mButtonUpHandlers;
	std::unordered_map<LLGL::Key, std::vector<std::function<void()>>> mButtonDownHandlers;
	std::vector<std::function<void(LLGL::Offset2D)>> mMouseMoveCallbacks;
	std::function<void()> mZoomInCallback;
	std::function<void()> mZoomOutCallback;
	std::shared_ptr<InputQueue> mInput; // User input event listener

	LLGL::Offset2D mCurrentMousePos = {0, 0};
};
