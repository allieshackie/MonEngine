#include "Core/RendererInstance.h"

#include "InputHandler.h"

InputHandler::InputHandler() {
	const auto& context = RendererInstance::GetInstance()->GetContext();
	mInput = std::make_shared<InputQueue>();
	auto& window = LLGL::CastTo<LLGL::Window>(context.GetSurface());
	window.AddEventListener(mInput);
}

void InputHandler::pollInputEvents() {
	while(mInput->HasEventsQueued())
	{
		switch (const auto event = mInput->ProcessNextEvent(); event.mKeyState)
		{
		case KeyStates::Key_Up:
			_handleButtonUpEvent(event.mKeyCode);
			break;

		default:
			break;
		}

		mInput->PopEvent();
	}
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
	mButtonUpHandlers.insert({ keyCode, callback });
}

void InputHandler::_handleButtonUpEvent(LLGL::Key keyCode)
{
	const auto& handler = mButtonUpHandlers.find(keyCode);
	if (handler != mButtonUpHandlers.end()) {
		handler->second();
	}
}
