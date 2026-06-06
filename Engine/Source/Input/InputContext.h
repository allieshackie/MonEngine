#pragma once

class GUISystem; 
class InputHandler;

class InputContext : public LLGL::Window::EventListener
{
public:
	InputContext(InputHandler& inputHandler, GUISystem& guiSystem);

protected:
	void OnKeyDown(LLGL::Window& sender, LLGL::Key keyCode) override;
	void OnKeyUp(LLGL::Window& sender, LLGL::Key keyCode) override;
	void OnWheelMotion(LLGL::Window& sender, int motion) override;
	void OnLocalMotion(LLGL::Window& sender, const LLGL::Offset2D& position) override;
	// Global motion is mouse direction, not screen position
	//void OnGlobalMotion(LLGL::Window& sender, const LLGL::Offset2D& position) override;
	void OnChar(LLGL::Window& sender, wchar_t chr) override;

private:
	InputHandler& mInputHandler;
	GUISystem& mGUISystem;
};
