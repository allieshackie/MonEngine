#include "Core/RendererInstance.h"
#include "Graphics/Core/Window.h"

#include "InputHandler.h"

std::unordered_map<LLGL::Key, std::vector<std::function<void()>>> InputHandler::mButtonUpHandlers;
std::unordered_map<LLGL::Key, std::vector<std::function<void()>>> InputHandler::mButtonDownHandlers;
std::vector<std::function<void(LLGL::Offset2D)>> InputHandler::mMouseMoveCallbacks;
std::function<void()> InputHandler::mZoomInCallback;
std::function<void()> InputHandler::mZoomOutCallback;

InputHandler::InputHandler(Window& window) {
	mInput = std::make_shared<InputQueue>();
	auto& llglWindow = window.GetWindow();
	llglWindow.AddEventListener(mInput);
}

void InputHandler::pollInputEvents() {
	while(mInput->HasEventsQueued())
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
}

void InputHandler::registerButtonUpHandler(LLGL::Key keyCode, const std::function<void()>& callback)
{
	const auto& handler = mButtonUpHandlers.find(keyCode);
	if (handler != mButtonUpHandlers.end()) {
		handler->second.push_back({ callback }); 
	}
	else
	{
		mButtonUpHandlers.insert({ keyCode, { callback } });
	}
}

void InputHandler::registerButtonDownHandler(LLGL::Key keyCode, const std::function<void()>& callback)
{
	const auto& handler = mButtonDownHandlers.find(keyCode);
	if (handler != mButtonDownHandlers.end()) {
		handler->second.push_back({ callback });
	}
	else
	{
		mButtonDownHandlers.insert({ keyCode, { callback } });
	}
}

void InputHandler::registerMouseMoveHandler(const std::function<void(LLGL::Offset2D)>& callback)
{
	mMouseMoveCallbacks.push_back(callback);
}

void InputHandler::registerZoomInHandler(const std::function<void()>& callback)
{
	mZoomInCallback = callback;
}

void InputHandler::registerZoomOutHandler(const std::function<void()>& callback)
{
	mZoomOutCallback = callback;
}

void InputHandler::_handleButtonUpEvent(LLGL::Key keyCode)
{
	const auto& handler = mButtonUpHandlers.find(keyCode);
	if (handler != mButtonUpHandlers.end()) {
		for (auto cb : handler->second)
		{
			cb();
		}
	}
}

void InputHandler::_handleButtonDownEvent(LLGL::Key keyCode)
{
	const auto& handler = mButtonDownHandlers.find(keyCode);
	if (handler != mButtonDownHandlers.end()) {
		for (auto cb : handler->second)
		{
			cb();
		}
	}
}

void InputHandler::_handleMouseMoveEvent()
{
	const auto mousePos = mInput->GetMousePosition();
	if (mousePos != mCurrentMousePos)
	{
		mCurrentMousePos = mousePos;
		for (const auto& fn : mMouseMoveCallbacks)
		{
			fn(mousePos);
		}
	}
}
