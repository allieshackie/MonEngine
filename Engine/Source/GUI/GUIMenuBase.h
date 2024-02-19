#pragma once
#include <vector>
#include <memory>

#include "GUIElement.h"

class GUIMenuBase
{
public:
	GUIMenuBase() = default;
	virtual ~GUIMenuBase() = default;
	// Since we define a destructor, rule of 5 states that we should define
	// all copy and move constructors/operators
	// Deleting to simplify ownership
	GUIMenuBase(GUIMenuBase& other) = delete;
	GUIMenuBase& operator=(GUIMenuBase& other) = delete;
	GUIMenuBase(GUIMenuBase&& other) = delete;
	GUIMenuBase& operator=(GUIMenuBase&& other) = delete;

	virtual void Render() = 0;

	void AddElement(std::unique_ptr<GUIElement> element)
	{
		mElements.push_back(std::move(element));
	}

protected:
	void Impl_Render() const
	{
		for (const auto& element : mElements)
		{
			element->Render();
		}
	}

private:
	std::vector<std::unique_ptr<GUIElement>> mElements;
};
