#include <filesystem>
#include "imgui.h"
#include "Core/ResourceManager.h"
#include "Core/RendererInstance.h"
#include "Tile.h"

#include "TileSetEditor.h"

namespace fs = std::filesystem;

static bool show_new_tileset_menu = false;
static bool show_load_texture_menu = false;
static bool show_current_sprite_data = false;
static int current_texture_selected = 0;

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

    if (ImGui::BeginMainMenuBar())
    {
        ImGui::MenuItem("New", nullptr, &show_new_tileset_menu);
        if (ImGui::BeginMenu("Load"))
        {
            if (ImGui::MenuItem("TileSet")) { std::cout << "load tile set" << std::endl; }
            ImGui::MenuItem("Texture", nullptr, &show_load_texture_menu);
            ImGui::EndMenu();
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
            static float posX = mCurrentSprite->GetPosition().x;
            ImGui::SliderFloat("Pos X", &posX, 1, 1000);

	    	if (posX != mCurrentSprite->GetPosition().x && !ImGui::IsItemActive())
	    	{
                mCurrentSprite->UpdatePositionX(posX);
	    	}

            static float posY = mCurrentSprite->GetPosition().y;
            ImGui::SliderFloat("Pos Y", &posY, 1, 1000);

            if (posY != mCurrentSprite->GetPosition().y && !ImGui::IsItemActive())
            {
                mCurrentSprite->UpdatePositionY(posY);
            }

	    	// SIZE
            static float sizeX = mCurrentSprite->GetSize().x;
            ImGui::SliderFloat("Size X", &sizeX, 1, 1000);

            if (sizeX != mCurrentSprite->GetSize().x && !ImGui::IsItemActive())
            {
                mCurrentSprite->UpdateSizeX(sizeX);
            }

            static float sizeY = mCurrentSprite->GetSize().y;
            ImGui::SliderFloat("Size Y", &sizeY, 1, 1000);

            if (sizeY != mCurrentSprite->GetSize().y && !ImGui::IsItemActive())
            {
                mCurrentSprite->UpdateSizeY(sizeY);
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

            static float clipX = mCurrentSprite->GetClip().x;
            ImGui::SliderFloat("Clip X", &clipX, -2, 2);

            if (clipX != mCurrentSprite->GetClip().x && !ImGui::IsItemActive())
            {
                mCurrentSprite->UpdateClipX(clipX);
            }

            static float clipY = mCurrentSprite->GetClip().y;
            ImGui::SliderFloat("Clip Y", &clipY, -2, 2);

            if (clipY != mCurrentSprite->GetClip().y && !ImGui::IsItemActive())
            {
                mCurrentSprite->UpdateClipY(clipY);
            }

            static float scaleX = mCurrentSprite->GetScale().x;
            ImGui::SliderFloat("Scale X", &scaleX, 0, 1);

            if (scaleX != mCurrentSprite->GetScale().x && !ImGui::IsItemActive())
            {
                mCurrentSprite->UpdateScaleX(scaleX);
            }

            static float scaleY = mCurrentSprite->GetScale().y;
            ImGui::SliderFloat("Scale Y", &scaleY, 0, 1);

            if (scaleY != mCurrentSprite->GetScale().y && !ImGui::IsItemActive())
            {
                mCurrentSprite->UpdateScaleY(scaleY);
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
