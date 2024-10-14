#pragma once

#include <queue>
#include <unordered_set>

#include "LLGL/Window.h"

class Camera;
class Window;

enum class KeyStates
{
	Key_Neutral = 0,
	Key_Down,
	Key_Up,
	Key_Hold,
	Char
};

struct InputEvent
{
	InputEvent() = default;

	InputEvent(LLGL::Key keyCode, KeyStates keyState): mKeyCode(keyCode), mKeyState(keyState)
	{
	}

	InputEvent(wchar_t newChar): mKeyState(KeyStates::Char), mChar(newChar)
	{
	}

	LLGL::Key mKeyCode = LLGL::Key::Any;
	KeyStates mKeyState = KeyStates::Key_Neutral;
	wchar_t mChar = '_';
};

struct HoldHandler
{
	std::function<void()> onHold;
	std::function<void()> onRelease;
};

class InputHandler : public LLGL::Window::EventListener
{
public:
	void RegisterButtonUpHandler(LLGL::Key keyCode, const std::function<void()>& callback);
	void RegisterButtonDownHandler(LLGL::Key keyCode, const std::function<void()>& callback);
	void RegisterButtonHoldHandler(LLGL::Key keyCode, const std::function<void()>& onHold,
	                               const std::function<void()>& onRelease);

	void RegisterMouseMoveHandler(const std::function<void(LLGL::Offset2D)>& callback);
	void RegisterZoomInHandler(const std::function<void()>& callback);
	void RegisterZoomOutHandler(const std::function<void()>& callback);

	void AddEditorInputs(Camera& camera);

	void Update();

protected:
	void OnKeyDown(LLGL::Window& sender, LLGL::Key keyCode) override;
	void OnKeyUp(LLGL::Window& sender, LLGL::Key keyCode) override;
	void OnWheelMotion(LLGL::Window& sender, int motion) override;
	void OnLocalMotion(LLGL::Window& sender, const LLGL::Offset2D& position) override;
	// Global motion is mouse direction, not screen position
	//void OnGlobalMotion(LLGL::Window& sender, const LLGL::Offset2D& position) override;
	void OnChar(LLGL::Window& sender, wchar_t chr) override;

private:
	void _handleKeyDown(LLGL::Key keyCode);
	void _handleKeyUp(LLGL::Key keyCode);

	void _handleKeyDownGUI(LLGL::Key keyCode);
	void _handleKeyUpGUI(LLGL::Key keyCode);

	std::unordered_map<LLGL::Key, std::vector<std::function<void()>>> mButtonUpHandlers;
	std::unordered_map<LLGL::Key, std::vector<std::function<void()>>> mButtonDownHandlers;
	std::unordered_map<LLGL::Key, std::vector<HoldHandler>> mButtonHoldHandlers;
	std::vector<std::function<void(LLGL::Offset2D)>> mMouseMoveCallbacks;
	std::vector<std::function<void(InputEvent)>> mMouseScrollCallbacks;
	std::function<void()> mZoomInCallback;
	std::function<void()> mZoomOutCallback;
	std::unordered_set<LLGL::Key> mKeysHeld;

	LLGL::Offset2D mCurrentMousePos = {0, 0};
};
