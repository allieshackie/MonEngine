#include "InputHandler.h"

InputHandler::InputHandler(LLGL::RenderContext& context) : mContext(context) {
	mInput = std::make_shared<LLGL::Input>();
	auto& window = LLGL::CastTo<LLGL::Window>(context.GetSurface());
	window.AddEventListener(mInput);
}

void InputHandler::pollInputEvents() {
	for (const auto& keyPair : mButtonDownHandlers)
	{
		if (mInput->KeyDown(keyPair.first))
		{
			keyPair.second();
		}
	}
}

void InputHandler::registerButtonDownHandler(LLGL::Key keyCode, const std::function<void()>& callback)
{
	mButtonDownHandlers.insert({ keyCode, callback });
}

void InputHandler::_handleButtonDownEvent(LLGL::Key keyCode)
{
	const auto& handler = mButtonDownHandlers.find(keyCode);
	if (handler != mButtonDownHandlers.end()) {
		handler->second();
	}
}
