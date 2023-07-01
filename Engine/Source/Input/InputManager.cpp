#include "InputHandler.h"

#include "InputManager.h"

InputManager::InputManager(InputHandler& handler) : mInputHandler(handler)
{
}

void InputManager::registerButtonUpHandler(LLGL::Key keyCode, const std::function<void()>& callback) const
{
	const auto& handler = mInputHandler.mButtonUpHandlers.find(keyCode);
	if (handler != mInputHandler.mButtonUpHandlers.end()) {
		handler->second.push_back({ callback });
	}
	else
	{
		mInputHandler.mButtonUpHandlers.insert({ keyCode, { callback } });
	}
}

void InputManager::registerButtonDownHandler(LLGL::Key keyCode, const std::function<void()>& callback) const
{
	const auto& handler = mInputHandler.mButtonDownHandlers.find(keyCode);
	if (handler != mInputHandler.mButtonDownHandlers.end()) {
		handler->second.push_back({ callback });
	}
	else
	{
		mInputHandler.mButtonDownHandlers.insert({ keyCode, { callback } });
	}
}

void InputManager::registerMouseMoveHandler(const std::function<void(LLGL::Offset2D)>& callback) const
{
	mInputHandler.mMouseMoveCallbacks.push_back(callback);
}

void InputManager::registerZoomInHandler(const std::function<void()>& callback) const
{
	mInputHandler.mZoomInCallback = callback;
}

void InputManager::registerZoomOutHandler(const std::function<void()>& callback) const
{
	mInputHandler.mZoomOutCallback = callback;
}