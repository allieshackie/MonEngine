#pragma once

#include <SDL.h>
#include <unordered_map>
#include <functional>

class InputHandler {
public:
	InputHandler();

	void tick();

	void registerEventHandler(SDL_EventType eventType, std::function<void()> callback);

	void registerButtonDownHandler(SDL_Keycode keyCode, std::function<void()> callback);

private:
	void _handleButtonDownEvent(SDL_Keycode keyCode);

	void _handleEvent(SDL_EventType eventType);

private:
	std::unordered_map<SDL_EventType, std::function<void()>> mEventHandlers;
	std::unordered_map<SDL_Keycode, std::function<void()>> mButtonDownHandlers;
	SDL_Event mProcessedEvent;
};