#include <glad/glad.h>
#include <imgui.h>
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_win32.h"
#include "LLGL/Platform/Win32/Win32NativeHandle.h"

#include "Graphics/RenderContext.h"
#include "Util/FileSystem.h"
#include "Util/LuaUtil.h"
#include "GUIMenu.h"
#include "GUITheme.h"

#include "GUISystem.h"

constexpr const char* GLSL_VERSION = "#version 460";

bool GUISystem::show_demo_window = false;

GUISystem::GUISystem(RenderContext& renderContext) : LuaBindable("GUISystem"), mRenderContext(renderContext)
{
	//Initialize Glad
	if (gladLoadGL() == 0)
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	const auto resolution = renderContext.GetResolution();
	io.DisplaySize = ImVec2{static_cast<float>(resolution.width), static_cast<float>(resolution.height)};

	mViewportSize = { resolution.width , resolution.height };

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Renderer backend
	LLGL::NativeHandle mainWindowHandle;
	renderContext.GetSurfaceNativeHandle(&mainWindowHandle, sizeof(mainWindowHandle));

	ImGui_ImplWin32_Init(mainWindowHandle.window);
	ImGui_ImplOpenGL3_Init(GLSL_VERSION);
}

void GUISystem::RenderMenus()
{
	bool hasChanged = false;
	if (mRenderContext.HasViewportSizeChanged()) 
	{
		hasChanged = true;
		mRenderContext.SetViewportSizeChanged(false);
		const auto resolution = mRenderContext.GetResolution();
		mViewportSize = { resolution.width , resolution.height };
	}
	for (auto it = mGUIMenus.begin(); it != mGUIMenus.end();)
	{
		if ((*it)->ShouldClose())
		{
			it = mGUIMenus.erase(it);
			continue;
		}
		(*it)->Render(mViewportSize, hasChanged);
		++it;
	}
}

void GUISystem::BindMethods(lua_State* state)
{
	LuaUtil::RegisterMethod<GUISystem, &GUISystem::CreatePopup>(state, "CreatePopup");
}

void GUISystem::BindInstanceGetter(lua_State* state)
{
	LuaUtil::RegisterInstanceGetter(state, "GetGUISystem", this);
}

void GUISystem::CloseGUI() const
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext();
}

void GUISystem::LoadGUITheme(const std::string& themeName) const
{
	ImGuiStyle& style = ImGui::GetStyle();
	const std::string guiPath = "GUI/" + themeName + ".json";
	const std::string fullPath = ASSETS_FOLDER + guiPath;
	const auto themeJson = FileSystem::ReadJson(fullPath);
	GUITheme theme;
	try
	{
		auto archive = FileSystem::CreateArchive(themeJson.dump());
		theme.serialize(archive);
	}
	catch (const cereal::Exception& e)
	{
		std::cerr << "GUITheme deserialization error: " << e.what() << std::endl;
		assert(false);
	}

	style.Alpha = theme.Alpha;
	style.DisabledAlpha = theme.DisabledAlpha;

	style.ChildRounding = theme.ChildRounding;
	style.GrabRounding = theme.GrabRounding;
	style.FrameRounding = theme.FrameRounding;
	style.PopupRounding = theme.PopupRounding;
	style.ScrollbarRounding = theme.ScrollbarRounding;
	style.TabRounding = theme.TabRounding;
	style.WindowRounding = theme.WindowRounding;

	style.CellPadding = theme.CellPadding;
	style.FramePadding = theme.FramePadding;
	style.WindowPadding = theme.WindowPadding;

	style.ChildBorderSize = theme.ChildBorderSize;
	style.FrameBorderSize = theme.FrameBorderSize;
	style.PopupBorderSize = theme.PopupBorderSize;
	style.WindowBorderSize = theme.WindowBorderSize;

	style.GrabMinSize = theme.GrabMinSize;
	style.ScrollbarSize = theme.ScrollbarSize;
	style.TabBorderSize = theme.TabBorderSize;
	style.WindowMinSize = theme.WindowMinSize;

	style.ButtonTextAlign = theme.ButtonTextAlign;
	style.SelectableTextAlign = theme.SelectableTextAlign;
	style.WindowTitleAlign = theme.WindowTitleAlign;

	style.ColorButtonPosition = theme.ColorButtonPosition;
	style.WindowMenuButtonPosition = theme.WindowMenuButtonPosition;

	style.ColumnsMinSpacing = theme.ColumnsMinSpacing;
	style.IndentSpacing = theme.IndentSpacing;
	style.ItemSpacing = theme.ItemSpacing;
	style.ItemInnerSpacing = theme.ItemInnerSpacing;

	style.Colors[ImGuiCol_Text] = theme.Colors[0];
	style.Colors[ImGuiCol_TextDisabled] = theme.Colors[1];

	style.Colors[ImGuiCol_WindowBg] = theme.Colors[2];
	style.Colors[ImGuiCol_ChildBg] = theme.Colors[3];
	style.Colors[ImGuiCol_PopupBg] = theme.Colors[4];
	style.Colors[ImGuiCol_FrameBg] = theme.Colors[5];
	style.Colors[ImGuiCol_FrameBgHovered] = theme.Colors[6];
	style.Colors[ImGuiCol_FrameBgActive] = theme.Colors[7];
	style.Colors[ImGuiCol_TitleBg] = theme.Colors[8];
	style.Colors[ImGuiCol_TitleBgCollapsed] = theme.Colors[9];
	style.Colors[ImGuiCol_TitleBgActive] = theme.Colors[10];
	style.Colors[ImGuiCol_MenuBarBg] = theme.Colors[11];
	style.Colors[ImGuiCol_ScrollbarBg] = theme.Colors[12];
	style.Colors[ImGuiCol_TextSelectedBg] = theme.Colors[13];
	style.Colors[ImGuiCol_ModalWindowDimBg] = theme.Colors[14];

	style.Colors[ImGuiCol_Border] = theme.Colors[15];
	style.Colors[ImGuiCol_BorderShadow] = theme.Colors[16];

	style.Colors[ImGuiCol_ScrollbarGrab] = theme.Colors[17];
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = theme.Colors[18];
	style.Colors[ImGuiCol_ScrollbarGrabActive] = theme.Colors[19];
	style.Colors[ImGuiCol_SliderGrab] = theme.Colors[20];
	style.Colors[ImGuiCol_SliderGrabActive] = theme.Colors[21];

	style.Colors[ImGuiCol_CheckMark] = theme.Colors[22];

	style.Colors[ImGuiCol_Button] = theme.Colors[23];
	style.Colors[ImGuiCol_ButtonHovered] = theme.Colors[24];
	style.Colors[ImGuiCol_ButtonActive] = theme.Colors[25];

	style.Colors[ImGuiCol_Header] = theme.Colors[26];
	style.Colors[ImGuiCol_HeaderHovered] = theme.Colors[27];
	style.Colors[ImGuiCol_HeaderActive] = theme.Colors[28];

	style.Colors[ImGuiCol_Separator] = theme.Colors[29];
	style.Colors[ImGuiCol_SeparatorHovered] = theme.Colors[30];
	style.Colors[ImGuiCol_SeparatorActive] = theme.Colors[31];

	style.Colors[ImGuiCol_ResizeGrip] = theme.Colors[32];
	style.Colors[ImGuiCol_ResizeGripHovered] = theme.Colors[33];
	style.Colors[ImGuiCol_ResizeGripActive] = theme.Colors[34];

	style.Colors[ImGuiCol_PlotLines] = theme.Colors[35];
	style.Colors[ImGuiCol_PlotLinesHovered] = theme.Colors[36];
	style.Colors[ImGuiCol_PlotHistogram] = theme.Colors[37];
	style.Colors[ImGuiCol_PlotHistogramHovered] = theme.Colors[38];
}

bool GUISystem::IsGUIContext() const
{
	const ImGuiIO& io = ImGui::GetIO();
	return io.WantCaptureKeyboard || io.WantCaptureMouse;
}

void GUISystem::DemoWindow() const
{
	ImGui::ShowDemoWindow(&show_demo_window);
}

void GUISystem::GUIStartFrame() const
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void GUISystem::GUIEndFrame() const
{
	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

std::shared_ptr<GUIMenu> GUISystem::CreatePopup(const std::string& name, glm::vec2 pos, glm::vec2 size)
{
	auto menu = std::make_shared<GUIMenu>(name, pos, size);

	mGUIMenus.push_back(menu);

	return menu;
}
