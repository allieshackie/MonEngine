#pragma once

class Camera;
class InputHandler;

class GUIBase
{
public:
	GUIBase() = default;
	virtual ~GUIBase() = default;

	virtual void RenderGUI() = 0;

	virtual void InitGUIInputs(InputHandler& inputHandler)
	{
	}
};
