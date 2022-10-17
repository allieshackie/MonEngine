#include "Graphics/Core/Window.h"

#include "InputHandler.h"

InputHandler::InputHandler(Window& window)
{
	mInput = std::make_shared<InputQueue>();
	auto& llglWindow = window.GetWindow();
	llglWindow.AddEventListener(mInput);
}

void InputHandler::pollInputEvents()
{
	while (mInput->HasEventsQueued())
	{
		switch (const auto event = mInput->ProcessNextEvent(); event.mKeyState)
		{
		case KeyStates::Key_Up:
			_handleButtonUpEvent(event.mKeyCode);
			break;

		case KeyStates::Key_Down:
			_handleButtonDownEvent(event.mKeyCode);
			break;

		default:
			break;
		}

		mInput->PopEvent();
	}

	if (!mInput->GetKeyDownArray().empty())
	{
		for (const auto key : mInput->GetKeyDownArray())
		{
			mInput->PushEvent(key, KeyStates::Key_Down);
		}
	}

	if (mInput->GetWheelMotion())
	{
		const auto scroll = mInput->GetWheelMotion();
		if (scroll > 0) // scroll up
		{
			if (mZoomInCallback)
			{
				mZoomInCallback();
			}
		}
		else
		{
			if (mZoomOutCallback)
			{
				mZoomOutCallback();
			}
		}
	}
	_handleMouseMoveEvent();
}

void InputHandler::pollGUIInputEvents(const std::function<void(const InputEvent& event)>& callback) const
{
	if (mInput->GetWheelMotion())
	{
		const auto scroll = mInput->GetWheelMotion();
		InputEvent event;
		event.mKeyCode = LLGL::Key::Zoom;
		if (scroll > 0) event.mKeyState = KeyStates::Key_Up;
		else event.mKeyState = KeyStates::Key_Down;

		callback(event);
	}
	while (mInput->HasEventsQueued())
	{
		const auto event = mInput->ProcessNextEvent();
		callback(event);
		mInput->PopEvent();
	}

	if (!mInput->GetKeyDownArray().empty())
	{
		for (const auto key : mInput->GetKeyDownArray())
		{
			mInput->PushEvent(key, KeyStates::Key_Down);
		}
	}
}

void InputHandler::_handleButtonUpEvent(LLGL::Key keyCode)
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

void InputHandler::_handleButtonDownEvent(LLGL::Key keyCode)
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

void InputHandler::_handleMouseMoveEvent()
{
	const auto& mousePos = mInput->GetMousePosition();
	if (mousePos != mCurrentMousePos)
	{
		mCurrentMousePos = mousePos;
		for (const auto& fn : mMouseMoveCallbacks)
		{
			fn(mousePos);
		}
	}
}
