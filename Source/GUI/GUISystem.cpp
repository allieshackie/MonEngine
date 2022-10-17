#include <glad/glad.h>
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_win32.h"

#include "Core/Renderer.h"
#include "InputHandler.h"

#include "GUISystem.h"


GUISystem::GUISystem()
{
	initGUI();
}

GUISystem::~GUISystem()
{
	closeGUI();
}

void GUISystem::initGUI()
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
	Renderer::GetInstance()->GetContext().GetSurface().GetNativeHandle(&mainWindowHandle, sizeof(mainWindowHandle));
	mNativeWindow = mainWindowHandle.window;

	ImGui_ImplWin32_Init(mainWindowHandle.window);
	ImGui_ImplOpenGL3_Init(GLSL_VERSION);
}

void GUISystem::closeGUI() const
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext();
}

void GUISystem::handleGUIInput(const InputEvent& inputEvent)
{
	switch (inputEvent.mKeyState)
	{
	case KeyStates::Key_Up:
		_handleButtonUp(inputEvent.mKeyCode);
		break;

	case KeyStates::Key_Down:
		_handleButtonDown(inputEvent.mKeyCode);
		break;
	case KeyStates::Char:
		{
			ImGuiIO& io = ImGui::GetIO();
			io.AddInputCharacterUTF16(inputEvent.mChar);
			break;
		}
	default:
		break;
	}
}

bool GUISystem::isGUIContext()
{
	ImGuiIO& io = ImGui::GetIO();
	return io.WantCaptureKeyboard || io.WantCaptureMouse;
}

void GUISystem::RenderGuiElements()
{
	ImGui::ShowDemoWindow(&show_demo_window);
}

void GUISystem::_handleButtonDown(const LLGL::Key keycode) const
{
	ImGuiIO& io = ImGui::GetIO();
	switch (keycode)
	{
	// Mouse buttons: 0=left, 1=right, 2=middle
	case LLGL::Key::LButton:
		{
			io.MouseDown[0] = true;
			break;
		}
	// Mouse buttons: 0=left, 1=right, 2=middle
	case LLGL::Key::RButton:
		{
			io.MouseDown[1] = true;
			break;
		}
	case LLGL::Key::Zoom:
		{
			io.MouseWheel += -1;
			break;
		}
	default:
		{
			if (static_cast<int>(keycode) < 256)
			{
				io.KeysDown[static_cast<int>(keycode)] = true;
			}
			break;
		}
	}
}

void GUISystem::_handleButtonUp(const LLGL::Key keycode) const
{
	ImGuiIO& io = ImGui::GetIO();
	switch (keycode)
	{
	// Mouse buttons: 0=left, 1=right, 2=middle
	case LLGL::Key::LButton:
		{
			io.MouseDown[0] = false;
			break;
		}
	// Mouse buttons: 0=left, 1=right, 2=middle
	case LLGL::Key::RButton:
		{
			io.MouseDown[1] = false;
			break;
		}
	case LLGL::Key::Zoom:
		{
			io.MouseWheel += 1;
			break;
		}
	default:
		{
			if (static_cast<int>(keycode) < 256)
			{
				io.KeysDown[static_cast<int>(keycode)] = false;
			}
			break;
		}
	}
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
