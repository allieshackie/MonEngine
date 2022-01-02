#pragma once

#include <LLGL/LLGL.h>

class InputHandler {
public:
	InputHandler(LLGL::RenderContext& context);

	void pollInputEvents();

	void registerButtonDownHandler(LLGL::Key keyCode, const std::function<void()>& callback);
private:
	void _handleButtonDownEvent(LLGL::Key keyCode);

	std::unordered_map<LLGL::Key, std::function<void()>> mButtonDownHandlers;
	std::shared_ptr<LLGL::Input> mInput; // User input event listener
	LLGL::RenderContext& mContext;
};