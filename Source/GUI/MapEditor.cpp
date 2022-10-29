#include <filesystem>
#include "imgui/imgui.h"
#include "Core/Renderer.h"
#include "Core/ResourceManager.h"
#include "InputManager.h"
#include "Map.h"
#include "Tile.h"

#include "MapEditor.h"

namespace fs = std::filesystem;

bool MapEditor::show_pallette_menu = false;
bool MapEditor::show_new_map_menu = false;
bool MapEditor::show_load_map_menu = false;

MapEditor::MapEditor(InputManager& inputManager)
{
	inputManager.registerMouseMoveHandler([this](LLGL::Offset2D mousePos) { _HandleMouseMove(mousePos); });
	inputManager.registerButtonDownHandler(LLGL::Key::LButton, [this]() { _OnClick(); });
	_GetAllMapFileNames();
}

void MapEditor::RenderGUI()
{
	if (show_new_map_menu) _NewMapMenu(&show_new_map_menu);
	if (show_load_map_menu) _LoadMapMenu(&show_load_map_menu);

	if (show_pallette_menu) _PalletteMenu(&show_pallette_menu);
}

void MapEditor::ShowNewMapMenu()
{
	show_new_map_menu = true;
}

void MapEditor::ShowLoadMapMenu()
{
	show_load_map_menu = true;
}

void MapEditor::_GetAllMapFileNames()
{
	mapFileNames.clear();
	for (const auto& entry : fs::directory_iterator(MAP_FOLDER))
	{
		mapFileNames.push_back(_strdup(entry.path().filename().string().c_str()));
	}

	tilesetFileNames.clear();
	for (const auto& entry : fs::directory_iterator(TILESET_FOLDER))
	{
		tilesetFileNames.push_back(_strdup(entry.path().filename().string().c_str()));
	}
}

// ====  MAP ====
void MapEditor::_NewMapMenu(bool* p_open)
{
	_CenterWindow(300, 300);
	if (ImGui::Begin("New Map", p_open, ImGuiWindowFlags_MenuBar))
	{
		static char buf1[64] = "";
		ImGui::InputText("File Name", buf1, IM_ARRAYSIZE(buf1));
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "Do not include spaces in name");

		ImGui::Spacing();
		static int current_tileset_selected = 0;
		ImGui::Combo("TileSets", &current_tileset_selected, tilesetFileNames.data(), tilesetFileNames.size());

		static int rows = 1;
		ImGui::InputInt("Rows", &rows, 1);
		static int columns = 1;
		ImGui::InputInt("Columns", &columns, 1);
		static int tileSize = 1;
		ImGui::InputInt("Tile Size", &tileSize, 1);

		if (ImGui::Button("Create"))
		{
			std::string newMapPath = MAP_FOLDER;
			newMapPath.append(buf1).append(".txt");
			std::ofstream fileOut(newMapPath);
			std::string tileSetName = tilesetFileNames[current_tileset_selected];
			// Search for the substring in string
			const std::string toErase = ".json";
			const size_t pos = tileSetName.find(toErase);
			if (pos != std::string::npos)
			{
				// If found then erase it from string
				tileSetName.erase(pos, toErase.length());
			}

			fileOut << tileSetName << "," << rows << "," << columns << "," << tileSize << "," << "\n";

			std::string tilesStr;
			for (int i = 0; i < rows; i++)
			{
				for (int j = 0; j < columns; j++)
				{
					tilesStr += "-1,";
				}
				fileOut << tilesStr;
				if (i != rows - 1)
				{
					fileOut << "\n";
				}
				tilesStr.clear();
			}

			fileOut.close();

			_LoadMap(buf1);

			show_new_map_menu = false;
		}
	}
	ImGui::End();
}

void MapEditor::_LoadMapMenu(bool* p_open)
{
	_CenterWindow(300, 300);
	if (ImGui::Begin("Load Map", p_open, ImGuiWindowFlags_MenuBar))
	{
		ImGui::Combo("Maps", &current_map_selected, mapFileNames.data(), mapFileNames.size());
		if (ImGui::Button("Open"))
		{
			_LoadMap(mapFileNames[current_map_selected]);
			show_load_map_menu = false;
		}
	}
	ImGui::End();
}

void MapEditor::_LoadMap(const char* mapName)
{
	mCurrentMap = std::make_shared<Map, glm::vec3>({100, 100, 0});
	mCurrentMap->Load(mapName);
	_CreateMapDebugGrid(*mCurrentMap);
	auto mapTexture = mCurrentMap->getTexturePath();
	ResourceManager::GetInstance()->AddRenderObjectToDrawList(mCurrentMap.get());
	if (ResourceManager::GetInstance()->CreateSimpleOpenGLTexture(
		mapTexture, &mPalletteTextureId, &mPalletteTextureWidth, &mPalletteTextureHeight))
	{
		show_pallette_menu = true;
	}
}

// ==== PALLETTE ====
void MapEditor::_PalletteMenu(bool* p_open)
{
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowSize(ImVec2(200, main_viewport->Size.y - 20));
	ImGui::SetNextWindowPos(ImVec2(0, 20));
	if (ImGui::Begin("Pallette", p_open, mWindowFlags))
	{
		ImGui::Text("Map Texture: ");
		ImGui::Image((ImTextureID)mPalletteTextureId,
		             ImVec2(mPalletteTextureWidth, mPalletteTextureHeight));

		ImGui::Dummy(ImVec2(0, 20));
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Brush Index: %d", current_brush_index);
		ImGui::SameLine();
		const static int totalTiles = mCurrentMap->GetTotalTileSetTiles();
		if (ImGui::ArrowButton("##up", ImGuiDir_Up))
		{
			if (current_brush_index < totalTiles - 1)
			{
				current_brush_index++;
			}
		}
		ImGui::SameLine();
		if (ImGui::ArrowButton("##down", ImGuiDir_Down))
		{
			if (current_brush_index > 0)
			{
				current_brush_index--;
			}
		}

		ImGui::Spacing();
		ImGui::Text("Current Brush: ");

		const auto texClip = mCurrentMap->GetClipForTile(current_brush_index);

		ImGui::Image((ImTextureID)mPalletteTextureId,
		             ImVec2(50, 50), ImVec2(texClip.x, texClip.y),
		             ImVec2(texClip.x + texClip.z, texClip.y + texClip.w));


		static bool saveChanges = false;
		if (ImGui::Button("Save"))
			saveChanges = true;
		if (saveChanges)
		{
			mCurrentMap->SaveTilesToFile();
			saveChanges = false;
		}
	}
	ImGui::End();
}

void MapEditor::_CreateMapDebugGrid(const Map& map)
{
	Renderer::GetInstance()->ClearDebugDraw();

	const auto pos = map.GetMapPosition();
	const auto size = map.GetMapSize();

	const glm::vec2 rowsColumns = map.GetMapRowsColumns();

	ResourceManager::GetInstance()->CreateGrid(pos, size, rowsColumns.x, rowsColumns.y, {255, 255, 255});
}

void MapEditor::_CenterWindow(float width, float height)
{
	// We specify a default position/size in case there's no data in the .ini file.
	// We only do it to make the demo applications a little more welcoming, but typically this isn't required.
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(main_viewport->Size.x / 2 - (width / 2), main_viewport->Size.y / 2 - (height / 2)));
	ImGui::SetNextWindowSize(ImVec2(width, height));
}

void MapEditor::_HandleMouseMove(LLGL::Offset2D mousePos)
{
	if (mCurrentMap != nullptr)
	{
		auto& tiles = mCurrentMap->GetTiles();
		for (int i = 0; i < tiles.size(); i++)
		{
			const auto pos = tiles[i]->GetPosition();
			const auto size = tiles[i]->GetSize();
			if (mousePos.x > pos.x &&
				mousePos.x < (pos.x + size.x * 2) &&
				mousePos.y > pos.y &&
				mousePos.y < (pos.y + size.y * 2))
			{
				if (current_hovered_tile_index != i)
				{
					current_hovered_tile_index = i;
				}
				return;
			}
		}
		current_hovered_tile_index = -1;
	}
}

void MapEditor::_OnClick() const
{
	if (mCurrentMap != nullptr && current_hovered_tile_index != -1)
	{
		mCurrentMap->UpdateTile(current_hovered_tile_index, current_brush_index);
	}
}
