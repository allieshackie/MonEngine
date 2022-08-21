#include <filesystem>
#include "imgui.h"
#include "Core/ResourceManager.h"
#include "Core/RendererInstance.h"
#include "Camera.h"
#include "Tile.h"

#include "TileSetEditor.h"

namespace fs = std::filesystem;

static bool show_new_tileset_menu = false;
static bool show_load_texture_menu = false;
static bool show_current_sprite_data = false;
static bool show_camera_info = false;
static int current_texture_selected = 0;

TileSetEditor::TileSetEditor(Camera& camera): mCamera(camera)
{
}

void TileSetEditor::RenderGUI()
{
    // Exceptionally add an extra assert here for people confused about initial Dear ImGui setup
   // Most ImGui functions would normally just crash if the context is missing.
    IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");

    // We specify a default position/size in case there's no data in the .ini file.
    // We only do it to make the demo applications a little more welcoming, but typically this isn't required.
    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

    if (show_new_tileset_menu) _NewTileSet(&show_new_tileset_menu);
    if (show_load_texture_menu) _LoadTextureMenu(&show_load_texture_menu);
    if (show_current_sprite_data) _CurrentSpriteMenu(&show_current_sprite_data);
    if (show_camera_info) _CameraInfo(&show_camera_info);

    if (ImGui::BeginMainMenuBar())
    {
        ImGui::MenuItem("New", nullptr, &show_new_tileset_menu);
        if (ImGui::BeginMenu("Load"))
        {
            if (ImGui::MenuItem("TileSet")) { std::cout << "load tile set" << std::endl; }
            ImGui::MenuItem("Texture", nullptr, &show_load_texture_menu);
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Toggle Camera Info"))
        {
            show_camera_info = !show_camera_info;
        }
        ImGui::EndMainMenuBar();
    }
}

void TileSetEditor::RenderTest()
{
    static bool demo = true;
    ImGui::ShowDemoWindow(&demo);
}

void TileSetEditor::_NewTileSet(bool* p_open)
{
    if (ImGui::Begin("New Tile Set", p_open, ImGuiWindowFlags_MenuBar))
    {
        static char buf1[64] = "";
    	ImGui::InputText("File Name", buf1, IM_ARRAYSIZE(buf1));

    	_GetTextureFileNames(mTextureFileNames);
        
        ImGui::Combo("Textures", &current_texture_selected, mTextureFileNames.data(), mTextureFileNames.size());

        static bool clicked = false;
        if (ImGui::Button("Create"))
            clicked = true;
        if (clicked)
        {
            show_new_tileset_menu = false;
            current_texture_selected = 0;
            clicked = false;
            _CreateNewTileSetFile(buf1, mTextureFileNames[current_texture_selected]);
        }
    }
    ImGui::End();
}

void TileSetEditor::_CreateNewTileSetFile(char* fileNameBuf, char* textureFileName)
{
    std::cout << fileNameBuf << std::endl;
    std::cout << textureFileName << std::endl;
	
}

void TileSetEditor::_LoadTextureMenu(bool* p_open)
{
    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Choose Texture", p_open, ImGuiWindowFlags_MenuBar))
    {
        _GetTextureFileNames(mTextureFileNames);
        ImGui::Combo("Textures", &current_texture_selected, mTextureFileNames.data(), mTextureFileNames.size());

        static bool clicked = false;
        if (ImGui::Button("Open"))
            clicked = true;
        if (clicked)
        {
            _LoadTileSetTexture(mTextureFileNames[current_texture_selected]);
            show_load_texture_menu = false;
            current_texture_selected = 0;
            show_current_sprite_data = true;
            clicked = false;
        }
    }
    ImGui::End();
}

void TileSetEditor::_CurrentSpriteMenu(bool* p_open)
{
	if (mCurrentSprite != nullptr)
	{
        ImGui::SetNextWindowSize(ImVec2(500, 550), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(0, 20));
	    if (ImGui::Begin("Tile Data", p_open, ImGuiWindowFlags_MenuBar))
	    {
            ImGui::Text("Transform");
	    	// POSITION
            static glm::vec2 pos = mCurrentSprite->GetPosition();
            ImGui::SliderFloat("Pos X", &pos.x, 1, 1000);
            ImGui::SliderFloat("Pos Y", &pos.y, 1, 1000);

            if (pos != mCurrentSprite->GetPosition() && !ImGui::IsItemActive())
            {
                mCurrentSprite->UpdatePosition(pos);
            }

	    	// SIZE
            static glm::vec2 size = mCurrentSprite->GetSize();
            ImGui::SliderFloat("Size X", &size.x, 1, 1000);
            ImGui::SliderFloat("Size Y", &size.y, 1, 1000);

            if (size != mCurrentSprite->GetSize() && !ImGui::IsItemActive())
            {
                mCurrentSprite->UpdateSize(size);
            }

	    	// ROTATION
            static float rot = mCurrentSprite->GetRotation();
            ImGui::SliderFloat("Rotation", &rot, 0, 360);

            if (rot != mCurrentSprite->GetRotation() && !ImGui::IsItemActive())
            {
                mCurrentSprite->UpdateRotation(rot);
            }


	    	// TEXTURE COORDS
            ImGui::Separator();
            ImGui::Text("Texture Coords");

            static glm::vec2 clip = mCurrentSprite->GetClip();
            ImGui::SliderFloat("Clip X", &clip.x, -2, 2);
            ImGui::SliderFloat("Clip Y", &clip.y, -2, 2);

            if (clip != mCurrentSprite->GetClip() && !ImGui::IsItemActive())
            {
                mCurrentSprite->UpdateClip(clip);
            }

            static glm::vec2 scale = mCurrentSprite->GetScale();
            ImGui::SliderFloat("Scale X", &scale.x, 0, 1);
            ImGui::SliderFloat("Scale Y", &scale.y, 0, 1);

            if (scale != mCurrentSprite->GetScale() && !ImGui::IsItemActive())
            {
                mCurrentSprite->UpdateScale(scale);
            }
	    }
	    ImGui::End();
	}
}

void TileSetEditor::_LoadTileSetTexture(const char* textureName)
{
    ResourceManager::CreateTile(textureName, { 200.0f, 200.0f }, { 100.0f, 100.0f });
    mCurrentSprite = ResourceManager::GetLatestTile();
}

void TileSetEditor::_GetTextureFileNames(std::array<char*, 6>& items)
{
    int i = 0;
	for (const auto& entry : fs::directory_iterator(TEXTURE_FOLDER_GUI))
	{
        items[i] = _strdup(entry.path().filename().string().c_str());
        i++;
	}
}

void TileSetEditor::_CameraInfo(bool* p_open)
{
    if (ImGui::Begin("Camera Info", p_open, ImGuiWindowFlags_MenuBar))
    {
        static glm::vec3 cameraFront = mCamera.GetFront();
        ImGui::SliderFloat("Front X", &cameraFront.x, -2, 2);
        ImGui::SliderFloat("Front Y", &cameraFront.y, -2, 2);
        ImGui::SliderFloat("Front Z", &cameraFront.z, -2, 2);

        if (cameraFront != mCamera.GetFront() && !ImGui::IsItemActive())
        {
            mCamera.SetFront(cameraFront);
        }

        static glm::vec3 cameraPos = mCamera.GetPosition();
        ImGui::SliderFloat("Pos X", &cameraPos.x, -2, 2);
        ImGui::SliderFloat("Pos Y", &cameraPos.y, -2, 2);
        ImGui::SliderFloat("Pos Z", &cameraPos.z, -2, 2);

        if (cameraPos != mCamera.GetPosition() && !ImGui::IsItemActive())
        {
            mCamera.SetPosition(cameraPos);
        }
    }
    ImGui::End();
}
