#pragma once
#include "LLGL/Key.h"
#include "LLGL/Types.h"

class InputHandler;

class InputManager
{
public:
	InputManager(InputHandler& handler);
	~InputManager() = default;

	void registerButtonUpHandler(LLGL::Key keyCode, const std::function<void()>& callback) const;
	void registerButtonDownHandler(LLGL::Key keyCode, const std::function<void()>& callback) const;

	void registerMouseMoveHandler(const std::function<void(LLGL::Offset2D)>& callback) const;
	void registerZoomInHandler(const std::function<void()>& callback) const;
	void registerZoomOutHandler(const std::function<void()>& callback) const;

private:
	InputHandler& mInputHandler;
};