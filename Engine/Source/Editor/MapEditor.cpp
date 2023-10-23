#include <filesystem>
#include <nlohmann/json.hpp>
#include "imgui.h"
#include "Core/Camera.h"
#include "Core/Level.h"
#include "Core/LevelManager.h"
#include "Graphics/RenderContext.h"
#include "Graphics/Core/ResourceManager.h"
#include "Map/Map.h"
#include "Map/MapAttributes.h"
#include "Map/MapRegistry.h"

#include "MapEditor.h"

MapEditor::MapEditor(EngineContext& engineContext, InputHandler& inputHandler, LevelManager& levelManager,
                     MapRegistry& mapRegistry, RenderContext& renderContext)
	: mInputHandler(inputHandler), mMapRegistry(mapRegistry), mRenderContext(renderContext)
{
	levelManager.LoadLevel("editor.json", engineContext);
	_GetAllMapFileNames();
}

void MapEditor::RenderGUI(const std::unique_ptr<Camera>& camera)
{
	if (show_new_map_menu) _NewMapMenu(&show_new_map_menu, camera);
	if (show_load_map_menu) _LoadMapMenu(&show_load_map_menu, camera);

	if (show_palette_menu) _PaletteMenu(&show_palette_menu);
	if (show_map_menu) _MapMenu(&show_map_menu);

	if (show_camera_info) _CameraInfo(&show_camera_info, camera);

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
	for (const auto& entry : std::filesystem::directory_iterator(MAPS_FOLDER))
	{
		if (entry.path().filename().string().find(".json") != std::string::npos)
		{
			mapFileNames.push_back(_strdup(entry.path().filename().string().c_str()));
		}
	}

	textureFileNames.clear();
	for (const auto& entry : std::filesystem::directory_iterator(TEXTURES_FOLDER))
	{
		textureFileNames.push_back(_strdup(entry.path().filename().string().c_str()));
	}
}

// ====  MAP ====
void MapEditor::_NewMapMenu(bool* p_open, const std::unique_ptr<Camera>& camera)
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
			mapJsonData[MapHelpers::ID_STRING] = id;
			mapJsonData[MapHelpers::ROWS_STRING] = rows;
			mapJsonData[MapHelpers::COLUMNS_STRING] = columns;

			mapJsonData[MapHelpers::TEXTURE_DATA_STRING] = {
				{MapHelpers::TEXTURE_PATH_STRING, textureFileNames[current_texture_selected]},
				{MapHelpers::ROWS_STRING, texMapRows},
				{MapHelpers::COLUMNS_STRING, texMapColumns},
			};

			std::string mapJson = MAPS_FOLDER;
			mapJson.append(fileName).append(".json");
			std::ofstream mapWrite(mapJson);
			mapWrite << mapJsonData;
			mapWrite.close();

			std::string mapTxt = MAPS_FOLDER;
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
			_LoadMap(openFileName.c_str(), camera);

			show_new_map_menu = false;
		}
	}
	ImGui::End();
}

void MapEditor::_LoadMapMenu(bool* p_open, const std::unique_ptr<Camera>& camera)
{
	_CenterWindow(300, 300);
	if (ImGui::Begin("Load Map", p_open, ImGuiWindowFlags_MenuBar))
	{
		ImGui::Combo("Maps", &current_map_selected, mapFileNames.data(), mapFileNames.size());
		if (ImGui::Button("Open"))
		{
			_LoadMap(mapFileNames[current_map_selected], camera);
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

void MapEditor::_LoadMap(const char* mapName, const std::unique_ptr<Camera>& camera)
{
	mMapRegistry.OpenMap(mapName);
	mMapInteractionSystem = std::make_unique<MapInteractionSystem>(camera, mInputHandler, mMapRegistry,
	                                                               mRenderContext);

	const auto& mapTexture = mMapRegistry.GetCurrentMap()->GetTexturePath();
	if (ResourceManager::CreateSimpleOpenGLTexture(
		mapTexture, &mPalletteTextureId, &mPalletteTextureWidth, &mPalletteTextureHeight))
	{
		assert(mMapInteractionSystem != nullptr);
		mMapInteractionSystem->SetPaletteBrush(current_brush_index);
		show_palette_menu = true;
		show_map_menu = true;
		draw_debug_map_grid = true;
	}
}

void MapEditor::_MapMenu(bool* p_open) const
{
	const auto& map = mMapRegistry.GetCurrentMap();
	ImGui::SetNextWindowSize(ImVec2(200, 200));
	ImGui::SetNextWindowPos(ImVec2(0, 20));
	if (ImGui::Begin("Map", p_open, mWindowFlags))
	{
		const auto mapPos = map->GetPosition();
		static float mapPosition[3] = {mapPos.x, mapPos.y, mapPos.z};
		if (ImGui::InputFloat3("Position", mapPosition))
		{
			map->SetPosition({mapPosition[0], mapPosition[1], mapPosition[2]});
		}

		ImGui::Separator();

		const auto mapRot = map->GetRotation();
		static float mapRotation[3] = {mapRot.x, mapRot.y, mapRot.z};
		if (ImGui::InputFloat3("Rotation", mapRotation))
		{
			map->SetRotation({mapRotation[0], mapRotation[1], mapRotation[2]});
		}

		ImGui::Separator();
		ImGui::Spacing();
		static int rows = map->GetRows();
		if (ImGui::InputInt("Rows", &rows))
		{
			map->SetTextureRows(rows);
		}

		static int columns = map->GetColumns();
		if (ImGui::InputInt("Columns", &columns))
		{
			map->SetTextureRows(rows);
		}

		if (ImGui::Button("Save"))
		{
			mMapRegistry.GetCurrentMap()->SaveTilesToFile();
		}
	}
	ImGui::End();
}

// ==== PALLETTE ====
void MapEditor::_PaletteMenu(bool* p_open)
{
	const auto& map = mMapRegistry.GetCurrentMap();
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
		const static int totalTiles = map->GetMapTextureRows() * map->GetMapTextureColumns();
		if (ImGui::ArrowButton("##up", ImGuiDir_Up))
		{
			if (current_brush_index < totalTiles - 1)
			{
				current_brush_index++;
			}
			mMapInteractionSystem->SetPaletteBrush(current_brush_index);
		}
		ImGui::SameLine();
		if (ImGui::ArrowButton("##down", ImGuiDir_Down))
		{
			if (current_brush_index > 0)
			{
				current_brush_index--;
			}
			mMapInteractionSystem->SetPaletteBrush(current_brush_index);
		}

		ImGui::Spacing();
		ImGui::Text("Current Brush: ");

		const auto texClip = map->GetClipForTile(current_brush_index);

		ImGui::Image((ImTextureID)mPalletteTextureId,
		             ImVec2(50, 50), ImVec2(texClip.x, texClip.y),
		             ImVec2(texClip.x + texClip.z, texClip.y + texClip.w));

		if (ImGui::Button("Save"))
		{
			map->SaveTilesToFile();
		}
		if (ImGui::Button("Close"))
		{
			mMapRegistry.CloseMap(mapFileNames[current_map_selected]);
			show_palette_menu = false;
			show_map_menu = false;
			draw_debug_map_grid = false;
		}
	}
	ImGui::End();
}

void MapEditor::_PreviewTexture(const char* mapName)
{
	mMapRegistry.OpenMap(mapName);

	// RenderDebug will render the map texture instead of the map tiles
	// TODO: Yeah we definitely want to handle this differently
	//mMapRegistry.GetCurrentMap()->SetRenderDebug(true);

	// Draw grid overlay on the texture to show how texture map is split by rows/columns
	draw_debug_texture_grid = true;
	show_texture_menu = true;
}

void MapEditor::_TextureMenu(bool* p_open) const
{
	const auto& map = mMapRegistry.GetCurrentMap();
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowSize(ImVec2(200, main_viewport->Size.y - 20));
	ImGui::SetNextWindowPos(ImVec2(0, 20));
	if (ImGui::Begin("Texture", p_open, mWindowFlags))
	{
		static int rows = map->GetMapTextureRows();
		if (ImGui::InputInt("Rows", &rows))
		{
			map->SetTextureRows(rows);
		}

		static int columns = map->GetMapTextureColumns();
		if (ImGui::InputInt("Columns", &columns))
		{
			map->SetTextureColumns(columns);
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

// TODO: Re-add Debug draw
void MapEditor::_DrawTextureDebugGrid() const
{
	const auto& map = mMapRegistry.GetCurrentMap();
	//DebugDrawManager::GetInstance()->DrawGrid(map->GetPosition(), map->GetMapSize(), {255, 0, 0},
	//            map->GetMapTextureRows(), map->GetMapTextureColumns());
}

void MapEditor::_DrawMapDebugGrid() const
{
	const auto& map = mMapRegistry.GetCurrentMap();
	//DebugDrawManager::GetInstance()->DrawGrid(map->GetPosition(), map->GetMapSize(), {255, 0, 0},
	//                                          map->GetRows(), map->GetColumns());
}


void MapEditor::_CameraInfo(bool* p_open, const std::unique_ptr<Camera>& camera) const
{
	if (camera == nullptr) return;
	if (ImGui::Begin("Camera Info", p_open, ImGuiWindowFlags_MenuBar))
	{
		const auto front = camera->GetFront();
		static float cameraFront[3] = {front.x, front.y, front.z};
		if (ImGui::SliderFloat3("Camera Front", cameraFront, -2, 2))
		{
			camera->SetFront({cameraFront[0], cameraFront[1], cameraFront[2]});
		}

		const auto cameraPos = camera->GetFront();
		static float cameraPosition[3] = {cameraPos.x, cameraPos.y, cameraPos.z};
		if (ImGui::SliderFloat3("Camera Position", cameraPosition, -100, 100))
		{
			camera->SetPosition({cameraPosition[0], cameraPosition[1], cameraPosition[2]});
		}
	}
	ImGui::End();
}
