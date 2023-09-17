#include <glad/glad.h>
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_win32.h"
#include "LLGL/Key.h"
#include "LLGL/Platform/Win32/Win32NativeHandle.h"

#include "Graphics/RenderContext.h"

#include "GUISystem.h"

constexpr const char* GLSL_VERSION = "#version 460";

bool GUISystem::show_demo_window = true;

void GUISystem::InitGUI(const RenderContext& renderContext)
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
	io.DisplaySize = {800, 600};

	io.KeyMap[ImGuiKey_Tab] = static_cast<int>(LLGL::Key::Tab);
	io.KeyMap[ImGuiKey_LeftArrow] = static_cast<int>(LLGL::Key::Left);
	io.KeyMap[ImGuiKey_RightArrow] = static_cast<int>(LLGL::Key::Right);
	io.KeyMap[ImGuiKey_UpArrow] = static_cast<int>(LLGL::Key::Up);
	io.KeyMap[ImGuiKey_DownArrow] = static_cast<int>(LLGL::Key::Down);
	io.KeyMap[ImGuiKey_PageUp] = static_cast<int>(LLGL::Key::PageUp);
	io.KeyMap[ImGuiKey_PageDown] = static_cast<int>(LLGL::Key::PageDown);
	io.KeyMap[ImGuiKey_Home] = static_cast<int>(LLGL::Key::Home);
	io.KeyMap[ImGuiKey_End] = static_cast<int>(LLGL::Key::End);
	io.KeyMap[ImGuiKey_Insert] = static_cast<int>(LLGL::Key::Insert);
	io.KeyMap[ImGuiKey_Delete] = static_cast<int>(LLGL::Key::Delete);
	io.KeyMap[ImGuiKey_Backspace] = static_cast<int>(LLGL::Key::Back);
	io.KeyMap[ImGuiKey_Space] = static_cast<int>(LLGL::Key::Space);
	io.KeyMap[ImGuiKey_Enter] = static_cast<int>(LLGL::Key::Return);
	io.KeyMap[ImGuiKey_Escape] = static_cast<int>(LLGL::Key::Escape);

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Renderer backend
	LLGL::NativeHandle mainWindowHandle;
	renderContext.GetNativeHandle(&mainWindowHandle, sizeof(mainWindowHandle));

	ImGui_ImplWin32_Init(mainWindowHandle.window);
	ImGui_ImplOpenGL3_Init(GLSL_VERSION);
}

void GUISystem::CloseGUI()
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext();
}

bool GUISystem::IsGUIContext()
{
	const ImGuiIO& io = ImGui::GetIO();
	return io.WantCaptureKeyboard || io.WantCaptureMouse;
}

void GUISystem::RenderGuiElements()
{
	ImGui::ShowDemoWindow(&show_demo_window);
}

void GUISystem::GUIStartFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void GUISystem::GUIEndFrame()
{
	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
