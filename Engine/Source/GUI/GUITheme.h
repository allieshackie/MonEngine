#pragma once

#include <imgui.h>

#include "Util/SerialUtil.h"

// Try to maintain 1-1 naming with imgui
struct GUITheme
{
	float Alpha;
	float DisabledAlpha;

	float ChildRounding;
	float GrabRounding;
	float FrameRounding;
	float PopupRounding;
	float ScrollbarRounding;
	float TabRounding;
	float WindowRounding;

	ImVec2 CellPadding;
	ImVec2 FramePadding;
	ImVec2 WindowPadding;

	float ChildBorderSize;
	float FrameBorderSize;
	float PopupBorderSize;
	float WindowBorderSize;

	float GrabMinSize;
	float ScrollbarSize;
	float TabBorderSize;
	ImVec2 WindowMinSize;

	ImVec2 ButtonTextAlign;
	ImVec2 SelectableTextAlign;
	ImVec2 WindowTitleAlign;

	int ColorButtonPosition;
	int WindowMenuButtonPosition;

	float ColumnsMinSpacing;
	float IndentSpacing;
	ImVec2 ItemSpacing;
	ImVec2 ItemInnerSpacing;

	std::vector<ImVec4> Colors;

	template <class Archive>
	void serialize(Archive& ar)
	{
		ar(CEREAL_NVP(Alpha),
		   CEREAL_NVP(DisabledAlpha),
		   CEREAL_NVP(ChildRounding),
		   CEREAL_NVP(GrabRounding),
		   CEREAL_NVP(FrameRounding),
		   CEREAL_NVP(PopupRounding),
		   CEREAL_NVP(ScrollbarRounding),
		   CEREAL_NVP(TabRounding),
		   CEREAL_NVP(WindowRounding),
		   CEREAL_NVP(CellPadding),
		   CEREAL_NVP(FramePadding),
		   CEREAL_NVP(WindowPadding),
		   CEREAL_NVP(ChildBorderSize),
		   CEREAL_NVP(FrameBorderSize),
		   CEREAL_NVP(PopupBorderSize),
		   CEREAL_NVP(WindowBorderSize),
		   CEREAL_NVP(GrabMinSize),
		   CEREAL_NVP(ScrollbarSize),
		   CEREAL_NVP(TabBorderSize),
		   CEREAL_NVP(WindowMinSize),
		   CEREAL_NVP(ButtonTextAlign),
		   CEREAL_NVP(SelectableTextAlign),
		   CEREAL_NVP(WindowTitleAlign),
		   CEREAL_NVP(ColorButtonPosition),
		   CEREAL_NVP(WindowMenuButtonPosition),
		   CEREAL_NVP(ColumnsMinSpacing),
		   CEREAL_NVP(IndentSpacing),
		   CEREAL_NVP(ItemSpacing),
		   CEREAL_NVP(ItemInnerSpacing),
		   CEREAL_NVP(Colors));
	}
};
