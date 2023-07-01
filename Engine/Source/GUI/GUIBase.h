#pragma once

class GUIBase
{
public:
	GUIBase() = default;
	virtual ~GUIBase() = default;

	virtual void RenderGUI() = 0;
};
