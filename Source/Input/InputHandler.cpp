#include "InputHandler.h"

InputHandler::InputHandler() {
	mProcessedEvent = {};
}

void InputHandler::tick() {
	//Handle events on queue
	while (SDL_PollEvent(&mProcessedEvent) != 0)
	{
		if (mProcessedEvent.type == SDL_KEYDOWN) {
			_handleButtonDownEvent(mProcessedEvent.key.keysym.sym);
		}
		else {
			_handleEvent((SDL_EventType)mProcessedEvent.type);
		}
	}
}

void InputHandler::registerEventHandler(SDL_EventType eventType, std::function<void()> callback) {
	mEventHandlers.insert({ eventType, callback });
}

void InputHandler::registerButtonDownHandler(SDL_Keycode keyCode, std::function<void()> callback) {
	mButtonDownHandlers.insert({ keyCode, callback });
}

void InputHandler::_handleButtonDownEvent(SDL_Keycode keyCode) {
	const auto& handler = mButtonDownHandlers.find(keyCode);
	if (handler != mButtonDownHandlers.end()) {
		handler->second();
	}
}

void InputHandler::_handleEvent(SDL_EventType eventType) {
	const auto& handler = mEventHandlers.find(eventType);
	if (handler != mEventHandlers.end()) {
		handler->second();
	}
}