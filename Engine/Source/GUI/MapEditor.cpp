#include <filesystem>
#include <nlohmann/json.hpp>
#include "imgui.h"
#include "Core/Camera.h"
#include "Core/Level.h"
#include "Core/LevelManager.h"
#include "Graphics/Core/RenderContext.h"
#include "Graphics/Core/ResourceManager.h"
#include "Graphics/Debug/DebugDraw.h"
#include "Input/InputManager.h"
#include "Map/Map.h"
#include "Map/MapDescription.h"
#include "Map/MapSystem.h"

#include "MapEditor.h"

MapEditor::MapEditor(InputManager& inputManager, LevelManager& levelManager, MapSystem& mapSystem,
                     RenderContext& renderContext, ResourceManager& resourceManager, std::string mapsFolderPath,
                     std::string texturesFolderPath)
	: mInputManager(inputManager), mMapSystem(mapSystem), mRenderContext(renderContext),
	  mResourceManager(resourceManager), mMapsFolderPath(std::move(mapsFolderPath)),
	  mTexturesFolderPath(std::move(texturesFolderPath))
{
	levelManager.LoadLevel("editor.json");
	// TODO: Get Camera from level for editor
	//mEditorCamera = levelManager.GetLevel("editor.json")->GetCamera();
	_GetAllMapFileNames();
}

void MapEditor::InitCameraInputs() const
{
	// Register camera handlers for moving the camera position
	// If the mCameraFront remains the same, this will result in the
	// camera view angling.  We can adjust the mCameraFront.xy to match the
	// camera position so that the view will not angle
	mInputManager.registerButtonUpHandler(LLGL::Key::Up, [=]() { mEditorCamera->MoveUp(); });
	mInputManager.registerButtonUpHandler(LLGL::Key::Down, [=]() { mEditorCamera->MoveDown(); });
	mInputManager.registerButtonUpHandler(LLGL::Key::Left, [=]() { mEditorCamera->MoveLeft(); });
	mInputManager.registerButtonUpHandler(LLGL::Key::Right, [=]() { mEditorCamera->MoveRight(); });

	// Handlers for handling the camera zoom
	mInputManager.registerZoomInHandler([=]() { mEditorCamera->ZoomIn(); });
	mInputManager.registerZoomOutHandler([=]() { mEditorCamera->ZoomOut(); });
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
	for (const auto& entry : std::filesystem::directory_iterator(mMapsFolderPath))
	{
		if (entry.path().filename().string().find(".json") != std::string::npos)
		{
			mapFileNames.push_back(_strdup(entry.path().filename().string().c_str()));
		}
	}

	textureFileNames.clear();
	for (const auto& entry : std::filesystem::directory_iterator(mTexturesFolderPath))
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

			mapJsonData[MapDescription::TEXTURE_DATA_STRING] = {
				{MapDescription::TEXTURE_PATH_STRING, textureFileNames[current_texture_selected]},
				{MapDescription::ROWS_STRING, texMapRows},
				{MapDescription::COLUMNS_STRING, texMapColumns},
			};

			std::string mapJson = mMapsFolderPath;
			mapJson.append(fileName).append(".json");
			std::ofstream mapWrite(mapJson);
			mapWrite << mapJsonData;
			mapWrite.close();

			std::string mapTxt = mMapsFolderPath;
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
	mMapSystem.CreateMapEditMode(mapName);
	mMapInteractionSystem = std::make_unique<MapInteractionSystem>(mEditorCamera, mInputManager, mMapSystem,
	                                                               mRenderContext);

	auto mapTexture = mMapSystem.GetCurrentMap()->GetMapDescription()->GetTexturePath();
	if (mResourceManager.CreateSimpleOpenGLTexture(
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
	const auto& map = mMapSystem.GetCurrentMap();
	const auto& mapDescription = map->GetMapDescription();
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
		static int rows = mapDescription->GetRows();
		if (ImGui::InputInt("Rows", &rows))
		{
			mapDescription->SetTextureMapRows(rows);
		}

		static int columns = mapDescription->GetColumns();
		if (ImGui::InputInt("Columns", &columns))
		{
			mapDescription->SetTextureMapRows(rows);
		}

		if (ImGui::Button("Save"))
		{
			mMapSystem.GetCurrentMap()->SaveTilesToFile();
		}
	}
	ImGui::End();
}

// ==== PALLETTE ====
void MapEditor::_PaletteMenu(bool* p_open)
{
	const auto& mapDescription = mMapSystem.GetCurrentMap()->GetMapDescription();
	const auto& map = mMapSystem.GetCurrentMap();
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
	mMapSystem.CreateMapEditMode(mapName);

	// RenderDebug will render the map texture instead of the map tiles
	mMapSystem.GetCurrentMap()->SetRenderDebug(true);

	// Draw grid overlay on the texture to show how texture map is split by rows/columns
	draw_debug_texture_grid = true;
	show_texture_menu = true;
}

void MapEditor::_TextureMenu(bool* p_open) const
{
	const auto& mapDescription = mMapSystem.GetCurrentMap()->GetMapDescription();
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
	const auto& map = mMapSystem.GetCurrentMap();
	const auto& mapDesc = map->GetMapDescription();
	DebugDrawManager::GetInstance()->DrawGrid(map->GetPosition(), map->GetMapSize(), {255, 0, 0},
	                                          mapDesc->GetTextureMapRows(), mapDesc->GetTextureMapColumns());
}

void MapEditor::_DrawMapDebugGrid() const
{
	const auto& map = mMapSystem.GetCurrentMap();
	const auto& mapDesc = map->GetMapDescription();
	DebugDrawManager::GetInstance()->DrawGrid(map->GetPosition(), map->GetMapSize(), {255, 0, 0},
	                                          mapDesc->GetRows(), mapDesc->GetColumns());
}


void MapEditor::_CameraInfo(bool* p_open) const
{
	if (mEditorCamera == nullptr) return;
	if (ImGui::Begin("Camera Info", p_open, ImGuiWindowFlags_MenuBar))
	{
		const auto front = mEditorCamera->GetFront();
		static float cameraFront[3] = {front.x, front.y, front.z};
		if (ImGui::SliderFloat3("Camera Front", cameraFront, -2, 2))
		{
			mEditorCamera->SetFront({cameraFront[0], cameraFront[1], cameraFront[2]});
		}

		const auto cameraPos = mEditorCamera->GetFront();
		static float cameraPosition[3] = {cameraPos.x, cameraPos.y, cameraPos.z};
		if (ImGui::SliderFloat3("Camera Position", cameraPosition, -100, 100))
		{
			mEditorCamera->SetPosition({cameraPosition[0], cameraPosition[1], cameraPosition[2]});
		}
	}
	ImGui::End();
}
