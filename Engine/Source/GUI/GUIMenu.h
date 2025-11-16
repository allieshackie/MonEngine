#pragma once
#include <imgui.h>
#include <glm/vec2.hpp>

#include "GUIMenuBase.h"

class GUIMenu : public GUIMenuBase
{
public:
	GUIMenu(const char* label, glm::vec2 pos, glm::vec2 size) : mLabel(label), mPosition(ImVec2(pos.x, pos.y)),
	                                                            mSize(ImVec2(size.x, size.y))
	{
	}

	void Render() override
	{
		if (mWindowFlags & ImGuiWindowFlags_NoMove)
		{
			const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(ImVec2(main_viewport->Size.x / 2 - (mSize.x / 2),
			                               main_viewport->Size.y / 2 - (mSize.y / 2)));
			// TODO: If we want to center the window vs position manually, still want to handle window resize event
			//ImGui::SetNextWindowPos(mPosition);
		}
		ImGui::SetNextWindowSize(mSize);
		if (ImGui::Begin(mLabel, &mOpen, mWindowFlags))
		{
			Impl_Render();
		}
		ImGui::End();
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

	ImGuiWindowFlags mWindowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
};
