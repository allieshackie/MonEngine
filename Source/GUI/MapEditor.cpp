#include <filesystem>
#include <nlohmann/json.hpp>
#include "imgui/imgui.h"
#include "Core/Renderer.h"
#include "Core/ResourceManager.h"
#include "Camera.h"
#include "Debug/DebugDraw.h"
#include "MapDescription.h"
#include "MapInteractionSystem.h"
#include "MapSystem.h"

#include "MapEditor.h"

namespace fs = std::filesystem;

MapEditor::MapEditor(ResourceManager& resourceManager, MapSystem& mapSystem, MapInteractionSystem& mapInteractionSystem,
                     Camera& camera)
	: mResourceManager(resourceManager), mMapSystem(mapSystem), mMapInteractionSystem(mapInteractionSystem),
	  mCamera(camera)
{
	_GetAllMapFileNames();
}

void MapEditor::RenderGUI()
{
	if (show_new_map_menu) _NewMapMenu(&show_new_map_menu);
	if (show_load_map_menu) _LoadMapMenu(&show_load_map_menu);

	if (show_palette_menu) _PaletteMenu(&show_palette_menu);
	if (show_map_menu) _MapMenu(&show_map_menu);

	if (show_camera_info) _CameraInfo(&show_camera_info);

	if (show_texture_menu) _TextureMenu(&show_texture_menu);
	if (draw_debug_texture_grid) _DrawTextureDebugGrid();
	if (draw_debug_map_grid) _DrawMapDebugGrid();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::MenuItem("New"))
		{
			show_new_map_menu = !show_new_map_menu;
		}
		if (ImGui::MenuItem("Load"))
		{
			show_load_map_menu = !show_load_map_menu;
		}
		if (ImGui::MenuItem("Toggle Camera Info"))
		{
			show_camera_info = !show_camera_info;
		}
		ImGui::EndMainMenuBar();
	}
}

void MapEditor::_GetAllMapFileNames()
{
	mapFileNames.clear();
	for (const auto& entry : fs::directory_iterator(MAP_FOLDER))
	{
		if (entry.path().filename().string().find(".json") != std::string::npos)
		{
			mapFileNames.push_back(_strdup(entry.path().filename().string().c_str()));
		}
	}

	textureFileNames.clear();
	for (const auto& entry : fs::directory_iterator(TEXTURES_FOLDER))
	{
		textureFileNames.push_back(_strdup(entry.path().filename().string().c_str()));
	}
}

// ====  MAP ====
void MapEditor::_NewMapMenu(bool* p_open)
{
	_CenterWindow(300, 300);
	if (ImGui::Begin("New Map", p_open, ImGuiWindowFlags_MenuBar))
	{
		static char fileName[64] = "";
		ImGui::InputText("File Name", fileName, IM_ARRAYSIZE(fileName));
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "Do not include spaces in name");

		ImGui::Spacing();
		static char id[64] = "";
		ImGui::InputText("Id", id, IM_ARRAYSIZE(id));

		ImGui::Spacing();
		static int rows = 1;
		ImGui::InputInt("Rows", &rows, 1);
		static int columns = 1;
		ImGui::InputInt("Columns", &columns, 1);
		static int tileSize = 1;
		ImGui::InputInt("Tile Size", &tileSize, 1);

		ImGui::Spacing();
		static float position[3] = {0, 0, 0};
		ImGui::InputFloat3("Position", position);
		static float size[3] = {0, 0, 0};
		ImGui::InputFloat3("Size", size);
		static float rotation = 0;
		ImGui::InputFloat("Rotation", &rotation);

		ImGui::Spacing();
		ImGui::Text("Texture Data:");
		static int current_texture_selected = 0;
		ImGui::Combo("Textures", &current_texture_selected, textureFileNames.data(), textureFileNames.size());
		// TODO: Show texture preview?
		static int texMapRows = 1;
		ImGui::InputInt("Texture Map Rows", &texMapRows, 1);
		static int texMapColumns = 1;
		ImGui::InputInt("Texture Map Columns", &texMapColumns, 1);

		if (ImGui::Button("Create"))
		{
			// Create map json file
			nlohmann::json mapJsonData;
			mapJsonData[MapDescription::ID_STRING] = id;
			mapJsonData[MapDescription::ROWS_STRING] = rows;
			mapJsonData[MapDescription::COLUMNS_STRING] = columns;
			mapJsonData[MapDescription::TILE_SIZE_STRING] = tileSize;

			mapJsonData[MapDescription::TRANSFORM_STRING] = {
				{MapDescription::POSITION_STRING, position},
				{MapDescription::SIZE_STRING, size},
				{MapDescription::ROTATION_STRING, rotation}
			};

			mapJsonData[MapDescription::TEXTURE_DATA_STRING] = {
				{MapDescription::TEXTURE_PATH_STRING, textureFileNames[current_texture_selected]},
				{MapDescription::ROWS_STRING, texMapRows},
				{MapDescription::COLUMNS_STRING, texMapColumns},
			};

			std::string mapJson = MAP_FOLDER;
			mapJson.append(fileName).append(".json");
			std::ofstream mapWrite(mapJson);
			mapWrite << mapJsonData;
			mapWrite.close();

			std::string mapTxt = MAP_FOLDER;
			mapTxt.append(fileName).append(".txt");
			std::ofstream dataWrite(mapTxt);
			std::string tilesStr;
			for (int i = 0; i < rows; i++)
			{
				for (int j = 0; j < columns; j++)
				{
					tilesStr += "-1,";
				}
				dataWrite << tilesStr;
				if (i != rows - 1)
				{
					dataWrite << "\n";
				}
				tilesStr.clear();
			}
			dataWrite.close();

			std::string openFileName = fileName;
			openFileName.append(".json");
			_LoadMap(openFileName.c_str());

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
		if (ImGui::Button("Preview Texture"))
		{
			_PreviewTexture(mapFileNames[current_map_selected]);
			show_load_map_menu = false;
		}
	}
	ImGui::End();
}

void MapEditor::_LoadMap(const char* mapName)
{
	mMapSystem.CreateMap(mapName);

	auto mapTexture = mMapSystem.GetCurrentMapDescription()->GetTexturePath();
	if (mResourceManager.CreateSimpleOpenGLTexture(
		mapTexture, &mPalletteTextureId, &mPalletteTextureWidth, &mPalletteTextureHeight))
	{
		mMapInteractionSystem.SetPaletteBrush(current_brush_index);
		show_palette_menu = true;
		show_map_menu = true;
		draw_debug_map_grid = true;
	}
}

void MapEditor::_MapMenu(bool* p_open)
{
	const auto mapDescription = mMapSystem.GetCurrentMapDescription();
	ImGui::SetNextWindowSize(ImVec2(200, 200));
	ImGui::SetNextWindowPos(ImVec2(0, 20));
	if (ImGui::Begin("Map", p_open, mWindowFlags))
	{
		const auto pos = mapDescription->GetPosition();
		static float mapPosition[3] = {pos.x, pos.y, pos.z};
		if (ImGui::InputFloat3("Position", mapPosition))
		{
			mapDescription->SetPosition({mapPosition[0], mapPosition[1], mapPosition[2]});
		}

		static float rotation = mapDescription->GetRotation();
		if (ImGui::InputFloat("Rotation", &rotation))
		{
			mapDescription->SetRotation(rotation);
		}

		ImGui::Separator();
		ImGui::Spacing();
		static int rows = mapDescription->GetRows();
		if (ImGui::InputInt("Rows", &rows))
		{
			//mapDescription->SetTextureMapRows(rows);
		}

		static int columns = mapDescription->GetColumns();
		if (ImGui::InputInt("Columns", &columns))
		{
			//mapDescription->SetTextureMapRows(rows);
		}

		static int tileSize = mapDescription->GetTileSize();
		if (ImGui::InputInt("Tile Size", &tileSize))
		{
			//mapDescription->SetTextureMapRows(rows);
		}

		if (ImGui::Button("Save"))
		{
			mapDescription->SaveTilesToFile();
		}
	}
	ImGui::End();
}

// ==== PALLETTE ====
void MapEditor::_PaletteMenu(bool* p_open)
{
	const auto mapDescription = mMapSystem.GetCurrentMapDescription();
	ImGui::SetNextWindowSize(ImVec2(200, 600));
	ImGui::SetNextWindowPos(ImVec2(0, 220));
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
		const static int totalTiles = mapDescription->GetTextureMapRows() * mapDescription->GetTextureMapColumns();
		if (ImGui::ArrowButton("##up", ImGuiDir_Up))
		{
			if (current_brush_index < totalTiles - 1)
			{
				current_brush_index++;
			}
			mMapInteractionSystem.SetPaletteBrush(current_brush_index);
		}
		ImGui::SameLine();
		if (ImGui::ArrowButton("##down", ImGuiDir_Down))
		{
			if (current_brush_index > 0)
			{
				current_brush_index--;
			}
			mMapInteractionSystem.SetPaletteBrush(current_brush_index);
		}

		ImGui::Spacing();
		ImGui::Text("Current Brush: ");

		const auto texClip = mapDescription->GetClipForTile(current_brush_index);

		ImGui::Image((ImTextureID)mPalletteTextureId,
		             ImVec2(50, 50), ImVec2(texClip.x, texClip.y),
		             ImVec2(texClip.x + texClip.z, texClip.y + texClip.w));

		if (ImGui::Button("Save"))
		{
			mapDescription->SaveTilesToFile();
		}
		if (ImGui::Button("Close"))
		{
			mMapSystem.CloseCurrentMap();
			show_palette_menu = false;
			show_map_menu = false;
			draw_debug_map_grid = false;
		}
	}
	ImGui::End();
}

void MapEditor::_PreviewTexture(const char* mapName)
{
	mMapSystem.CreateMap(mapName);

	// RenderDebug will render the map texture instead of the map tiles
	mMapSystem.GetCurrentMapDescription()->SetRenderDebug(true);

	// Draw grid overlay on the texture to show how texture map is split by rows/columns
	draw_debug_texture_grid = true;
	show_texture_menu = true;
}

void MapEditor::_TextureMenu(bool* p_open) const
{
	const auto mapDescription = mMapSystem.GetCurrentMapDescription();
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowSize(ImVec2(200, main_viewport->Size.y - 20));
	ImGui::SetNextWindowPos(ImVec2(0, 20));
	if (ImGui::Begin("Texture", p_open, mWindowFlags))
	{
		static int rows = mapDescription->GetTextureMapRows();
		if (ImGui::InputInt("Rows", &rows))
		{
			mapDescription->SetTextureMapRows(rows);
		}

		static int columns = mapDescription->GetTextureMapColumns();
		if (ImGui::InputInt("Columns", &columns))
		{
			mapDescription->SetTextureMapColumns(columns);
		}

		static bool saveChanges = false;
		if (ImGui::Button("Save"))
			saveChanges = true;
		if (saveChanges)
		{
			// TODO: Save Changes
			//mCurrentMap->SaveTilesToFile();
			saveChanges = false;
		}
	}
	ImGui::End();
}

void MapEditor::_CenterWindow(float width, float height)
{
	// We specify a default position/size in case there's no data in the .ini file.
	// We only do it to make the demo applications a little more welcoming, but typically this isn't required.
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(main_viewport->Size.x / 2 - (width / 2), main_viewport->Size.y / 2 - (height / 2)));
	ImGui::SetNextWindowSize(ImVec2(width, height));
}

void MapEditor::_DrawTextureDebugGrid() const
{
	const auto map = mMapSystem.GetCurrentMapDescription();
	DebugDrawManager::GetInstance()->DrawGrid(map->GetPosition(), map->GetMapSize(), {255, 0, 0},
	                                          map->GetTextureMapRows(), map->GetTextureMapColumns());
}

void MapEditor::_DrawMapDebugGrid() const
{
	const auto map = mMapSystem.GetCurrentMapDescription();
	DebugDrawManager::GetInstance()->DrawGrid(map->GetPosition(), map->GetMapSize(), {255, 0, 0},
	                                          map->GetRows(), map->GetColumns());
}


void MapEditor::_CameraInfo(bool* p_open) const
{
	if (ImGui::Begin("Camera Info", p_open, ImGuiWindowFlags_MenuBar))
	{
		const auto front = mCamera.GetFront();
		static float cameraFront[3] = {front.x, front.y, front.z};
		if (ImGui::SliderFloat3("Camera Front", cameraFront, -2, 2))
		{
			mCamera.SetFront({cameraFront[0], cameraFront[1], cameraFront[2]});
		}

		const auto cameraPos = mCamera.GetFront();
		static float cameraPosition[3] = {cameraPos.x, cameraPos.y, cameraPos.z};
		if (ImGui::SliderFloat3("Camera Position", cameraPosition, -100, 100))
		{
			mCamera.SetPosition({cameraPosition[0], cameraPosition[1], cameraPosition[2]});
		}
	}
	ImGui::End();
}
