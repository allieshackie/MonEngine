#pragma once

#include <queue>
#include <unordered_set>
#include <imgui.h>

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

using HandlerId = size_t;

struct HandlerEntry {
	HandlerId mId;
	std::function<void()> mCallback;
};
struct HoldHandlerEntry
{
	HandlerId mId;
	std::function<void()> onHold;
	std::function<void()> onRelease;
};
struct MouseMoveHandlerEntry
{
	HandlerId mId;
	std::function<void(LLGL::Offset2D)> mCallback;
};

class InputHandler
{
public:
	HandlerId RegisterButtonUpHandler(LLGL::Key keyCode, const std::function<void()>& callback);
	HandlerId RegisterButtonDownHandler(LLGL::Key keyCode, const std::function<void()>& callback);
	HandlerId RegisterButtonHoldHandler(LLGL::Key keyCode, const std::function<void()>& onHold,
	                               const std::function<void()>& onRelease);

	HandlerId RegisterMouseMoveHandler(const std::function<void(LLGL::Offset2D)>& callback);
	void RegisterZoomInHandler(const std::function<void()>& callback);
	void RegisterZoomOutHandler(const std::function<void()>& callback);

	void Unregister(HandlerId id);
	void UnregisterZoomHandlers();

	const std::vector<LLGL::Key>& GetKeysPressed() const { return mDebugKeysPressed; }
	const std::vector<LLGL::Key>& GetPreviousKeysHeld() const { return mPreviousKeysPressed; }
	const char* GetNameForKey(LLGL::Key key) const;

	void AddEditorInputs(Camera& camera);

	void Update();

	void HandleKeyDown(LLGL::Key keyCode);
	void HandleKeyUp(LLGL::Key keyCode);

	void HandleKeyDownGUI(LLGL::Key keyCode);
	void HandleKeyUpGUI(LLGL::Key keyCode);

	void TriggerZoomInCallback();
	void TriggerZoomOutCallback();
	void TriggerMouseMoveCallbacks(const LLGL::Offset2D& position);

	void AddToDebugKeys(LLGL::Key keyCode);
	void RemoveFromDebugKeys(LLGL::Key keyCode);

private:
	ImGuiKey LLGLKeyToImGuiKey(LLGL::Key key);

	std::unordered_map<LLGL::Key, std::vector<HandlerEntry>> mButtonUpHandlers;
	std::unordered_map<LLGL::Key, std::vector<HandlerEntry>> mButtonDownHandlers;
	std::unordered_map<LLGL::Key, std::vector<HoldHandlerEntry>> mButtonHoldHandlers;
	std::vector<MouseMoveHandlerEntry> mMouseMoveCallbacks;
	std::function<void()> mZoomInCallback;
	std::function<void()> mZoomOutCallback;
	std::unordered_set<LLGL::Key> mKeysHeld;

	HandlerId mID = 0;

	LLGL::Offset2D mCurrentMousePos = {0, 0};

	// GUI Menu
	std::vector<LLGL::Key> mDebugKeysPressed;
	std::vector<LLGL::Key> mPreviousKeysPressed;

	// Only naming the keys we're most concerned with
	std::unordered_map<LLGL::Key, const char*> mKeyNames = {
		{LLGL::Key::W, "W"},
		{LLGL::Key::A, "A"},
		{LLGL::Key::S, "S"},
		{LLGL::Key::D, "D"},
		{LLGL::Key::Space, "Space"},
		{LLGL::Key::C, "C"},
	};
};
