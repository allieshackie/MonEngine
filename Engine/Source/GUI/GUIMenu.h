#pragma once
#include <imgui.h>

#include "GUIMenuBase.h"
#include "GUIButton.h"

class GUIMenu : public GUIMenuBase
{
public:
	GUIMenu(const char* label, glm::vec2 pos, glm::vec2 size) : mLabel(label), mPosition(ImVec2(pos.x, pos.y)),
	                                                            mSize(ImVec2(size.x, size.y))
	{
	}

	void Render() override
	{
		ImGui::SetNextWindowPos(mPosition);
		ImGui::SetNextWindowSize(mSize);
		if (ImGui::Begin(mLabel, &mOpen, mWindowFlags))
		{
			Impl_Render();
		}
		ImGui::End();
	}

	void AddButton(const char* label, float posX, float posY, float sizeX, float sizeY,
	               std::function<void()> clickCallback)
	{
		auto button = std::make_unique<GUIButton>(label, glm::vec2{posX, posY}, glm::vec2{sizeX, sizeY},
		                                          std::move(clickCallback));
		AddElement(std::move(button));
	}

	void SetNoMoveFlag(bool noMove)
	{
		if (noMove)
		{
			// If ImGuiWindowFlags_NoMove is not set
			if (!(mWindowFlags & ImGuiWindowFlags_NoMove))
			{
				// Set ImGuiWindowFlags_NoMove flag
				mWindowFlags |= ImGuiWindowFlags_NoMove;
			}
		}
		else
		{
			// if flag is set
			if (mWindowFlags & ImGuiWindowFlags_NoMove)
			{
				// unset
				mWindowFlags &= ~ImGuiWindowFlags_NoMove;
			}
		}
	}

	void SetNoResizeFlag(bool noResize)
	{
		if (noResize)
		{
			// If ImGuiWindowFlags_NoResize is not set
			if (!(mWindowFlags & ImGuiWindowFlags_NoResize))
			{
				// Set ImGuiWindowFlags_NoResize flag
				mWindowFlags |= ImGuiWindowFlags_NoResize;
			}
		}
		else
		{
			// if flag is set
			if (mWindowFlags & ImGuiWindowFlags_NoResize)
			{
				// unset
				mWindowFlags &= ~ImGuiWindowFlags_NoResize;
			}
		}
	}

private:
	bool mOpen = true;
	const char* mLabel;
	ImVec2 mPosition = {0, 0};
	ImVec2 mSize = {0, 0};

	ImGuiWindowFlags mWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
};
