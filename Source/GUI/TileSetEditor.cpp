#include <filesystem>
#include "imgui/imgui.h"
#include "Core/Renderer.h"
#include "Core/ResourceManager.h"
#include "TileSetDescription.h"
#include "Camera.h"
#include "MapEditor.h"

#include "TileSetEditor.h"

namespace fs = std::filesystem;

TileSetEditor::TileSetEditor(Renderer& renderer, ResourceManager& resourceManager, Camera& camera)
	: mCamera(camera), mRenderer(renderer), mResourceManager(resourceManager)
{
	_GetTextureFileNames();
	_GetTilesetFileNames();
}

void TileSetEditor::RenderGUI()
{
	// Exceptionally add an extra assert here for people confused about initial Dear ImGui setup
	// Most ImGui functions would normally just crash if the context is missing.
	IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");

	if (show_new_tileset_menu) _NewTileSet(&show_new_tileset_menu);
	if (show_load_tileset_menu) _LoadTileSetMenu(&show_load_tileset_menu);
	if (show_load_texture_menu) _LoadTextureMenu(&show_load_texture_menu);


	if (show_tileset_info_menu) _TileSetInfoMenu(&show_tileset_info_menu);
	if (show_texture_display_data) _TextureDisplayMenu(&show_texture_display_data);

	if (show_camera_info) _CameraInfo(&show_camera_info);

	//static bool show_app_metrics = true;
	//ImGui::ShowMetricsWindow(&show_app_metrics);

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("New"))
		{
			ImGui::MenuItem("TileSet", nullptr, &show_new_tileset_menu);
			if (ImGui::MenuItem("Map")) MapEditor::ShowNewMapMenu();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Load"))
		{
			ImGui::MenuItem("TileSet", nullptr, &show_load_tileset_menu);
			if (ImGui::MenuItem("Map")) MapEditor::ShowLoadMapMenu();
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

// ===== FILE DISPLAY HELPERS =====
void TileSetEditor::_GetTilesetFileNames()
{
	tilesetFileNames.clear();
	for (const auto& entry : fs::directory_iterator(TILESET_FOLDER))
	{
		tilesetFileNames.push_back(_strdup(entry.path().filename().string().c_str()));
	}
}

void TileSetEditor::_GetTextureFileNames()
{
	textureFileNames.clear();
	for (const auto& entry : fs::directory_iterator(TEXTURE_FOLDER_GUI))
	{
		textureFileNames.push_back(_strdup(entry.path().filename().string().c_str()));
	}
}

// ===== NEW TILESET =====
void TileSetEditor::_NewTileSet(bool* p_open)
{
	if (ImGui::Begin("New Tile Set", p_open, ImGuiWindowFlags_MenuBar))
	{
		static char buf1[64] = "";
		ImGui::InputText("File Name", buf1, IM_ARRAYSIZE(buf1));

		ImGui::Combo("Textures", &current_texture_selected, textureFileNames.data(), textureFileNames.size());

		static bool clicked = false;
		if (ImGui::Button("Create"))
			clicked = true;
		if (clicked)
		{
			show_new_tileset_menu = false;
			current_texture_selected = 0;
			clicked = false;
			_CreateNewTileSetFile(buf1, textureFileNames[current_texture_selected]);
		}
	}
	ImGui::End();
}

void TileSetEditor::_CreateNewTileSetFile(char* fileNameBuf, char* textureFileName)
{
	std::cout << fileNameBuf << std::endl;
	std::cout << textureFileName << std::endl;
}

// ===== LOAD TILESET =====
void TileSetEditor::_LoadTileSetMenu(bool* p_open)
{
	ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Choose Tileset File", p_open, ImGuiWindowFlags_MenuBar))
	{
		ImGui::Combo("Tilesets", &current_tileset_selected, tilesetFileNames.data(), tilesetFileNames.size());

		static bool clicked = false;
		if (ImGui::Button("Open"))
			clicked = true;
		if (clicked)
		{
			//mCurrentTileset = std::make_shared<TileSetDescription>(tilesetFileNames[current_tileset_selected]);
			if (mCurrentTileset == nullptr) return;
			_LoadTileSetTexture(mCurrentTileset->getTexturePath().c_str());
			show_texture_display_data = true;
			show_tileset_info_menu = true;
			show_load_tileset_menu = false;
			current_tileset_selected = 0;
			clicked = false;
		}
	}
	ImGui::End();
}

void TileSetEditor::_TileSetInfoMenu(bool* p_open)
{
	if (mCurrentTileset == nullptr) return;
	ImGui::SetNextWindowSize(ImVec2(200, 150));
	ImGui::SetNextWindowPos(ImVec2(0, 20));
	if (ImGui::Begin("Tileset Info", p_open, mWindowFlags))
	{
		ImGui::Text("Texture Name: ");
		ImGui::SameLine();
		ImGui::Text(mCurrentTileset->getTexturePath().c_str());

		static int rows = mCurrentTileset->GetRows();
		ImGui::InputInt("Rows", &rows);

		static int columns = mCurrentTileset->GetColumns();
		ImGui::InputInt("Columns", &columns);

		if (rows != mCurrentTileset->GetRows() || columns != mCurrentTileset->GetColumns())
		{
			static bool saveButtonClicked = false;
			if (ImGui::Button("Save"))
				saveButtonClicked = true;
			if (saveButtonClicked)
			{
				mCurrentTileset->UpdateSize(rows, columns);
				saveButtonClicked = false;
			}
		}
	}
	ImGui::End();
}

// ===== TEXTURE DISPLAY =====
void TileSetEditor::_LoadTextureMenu(bool* p_open)
{
	ImGui::SetNextWindowSize(ImVec2(500, 440));
	if (ImGui::Begin("Choose Texture", p_open, ImGuiWindowFlags_MenuBar))
	{
		ImGui::Combo("Textures", &current_texture_selected, textureFileNames.data(), textureFileNames.size());

		static bool clicked = false;
		if (ImGui::Button("Open"))
			clicked = true;
		if (clicked)
		{
			_LoadTileSetTexture(textureFileNames[current_texture_selected]);
			show_load_texture_menu = false;
			current_texture_selected = 0;
			show_texture_display_data = true;
			clicked = false;
		}
	}
	ImGui::End();
}

void TileSetEditor::_LoadTileSetTexture(const char* textureName)
{
	//mResourceManager.AddSprite(textureName, {0, 0, 10}, {5, 5, 1}, "tile-set-editor");
	//mCurrentSprite = mResourceManager.GetDrawDataById("tile-set-editor");
}

void TileSetEditor::_TextureDisplayMenu(bool* p_open)
{
	/*
	 *
	if (mCurrentSprite != nullptr)
	{
		ImGui::SetNextWindowSize(ImVec2(200, 350));
		ImGui::SetNextWindowPos(ImVec2(0, 170));
		if (ImGui::Begin("Tile Data", p_open, mWindowFlags))
		{
			ImGui::Text("Transform");
			// POSITION
			static glm::vec3 pos = mCurrentSprite->mPosition;
			ImGui::SliderFloat("Pos X", &pos.x, -1000, 1000);
			ImGui::SliderFloat("Pos Y", &pos.y, -1000, 1000);

			if (pos != mCurrentSprite->mPosition && !ImGui::IsItemActive())
			{
				mCurrentSprite->mPosition = pos;
			}

			// SIZE
			static glm::vec3 size = mCurrentSprite->mSize;
			ImGui::SliderFloat("Size X", &size.x, 1, 100);
			ImGui::SliderFloat("Size Y", &size.y, 1, 100);

			if (size != mCurrentSprite->mSize && !ImGui::IsItemActive())
			{
				mCurrentSprite->mSize = size;
			}

			// ROTATION
			static float rot = mCurrentSprite->mRotation;
			ImGui::SliderFloat("Rotation", &rot, 0, 360);

			if (rot != mCurrentSprite->mRotation && !ImGui::IsItemActive())
			{
				mCurrentSprite->mRotation = rot;
			}

			// TEXTURE COORDS
			ImGui::Separator();
			ImGui::Text("Texture Coords");

			static glm::vec2 clip = mCurrentSprite->mTexClip;
			ImGui::SliderFloat("Clip X", &clip.x, -2, 2);
			ImGui::SliderFloat("Clip Y", &clip.y, -2, 2);

			if (clip != mCurrentSprite->mTexClip && !ImGui::IsItemActive())
			{
				mCurrentSprite->mTexClip = clip;
			}

			static glm::vec2 scale = mCurrentSprite->mTexScale;
			ImGui::SliderFloat("Scale X", &scale.x, 0, 1);
			ImGui::SliderFloat("Scale Y", &scale.y, 0, 1);

			if (scale != mCurrentSprite->mTexScale && !ImGui::IsItemActive())
			{
				mCurrentSprite->mTexScale = scale;
			}
		}
		ImGui::End();
	}
	 */
}

void TileSetEditor::_CameraInfo(bool* p_open) const
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
		ImGui::SliderFloat("Pos Z", &cameraPos.z, -1000, 10);

		if (cameraPos != mCamera.GetPosition() && !ImGui::IsItemActive())
		{
			mCamera.SetPosition(cameraPos);
		}
	}
	ImGui::End();
}

void TileSetEditor::_CenterWindow(float width, float height)
{
	// We specify a default position/size in case there's no data in the .ini file.
	// We only do it to make the demo applications a little more welcoming, but typically this isn't required.
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(main_viewport->Size.x / 2 - (width / 2), main_viewport->Size.y / 2 - (height / 2)));
	ImGui::SetNextWindowSize(ImVec2(width, height));
}
