#pragma once

class GUIElement
{
public:
	GUIElement() = default;
	virtual ~GUIElement() = default;
	// Since we define a destructor, rule of 5 states that we should define
	// all copy and move constructors/operators
	// Deleting to simplify ownership
	GUIElement(GUIElement& other) = delete;
	GUIElement& operator=(GUIElement& other) = delete;
	GUIElement(GUIElement&& other) = delete;
	GUIElement& operator=(GUIElement&& other) = delete;

	virtual void Render() = 0;
};
