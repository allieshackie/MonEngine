#include "Input/InputHandler.h"
#include "Core/Scene.h"
#include "Core/SceneManager.h"
#include "Entity/Components/TransformComponent.h"
#include "Entity/Entity.h"
#include "Graphics/RenderContext.h"
#include "Graphics/Core/ResourceManager.h"

#include "EntityMenu.h"

EntityMenu::EntityMenu(const SceneManager& sceneManager, InputHandler& inputHandler)
{
	// TODO: Handle mouse hover + selection 
	inputHandler.RegisterMouseMoveHandler([this](LLGL::Offset2D mousePos) { _HandleMouseMove(mousePos); });
	inputHandler.RegisterButtonDownHandler(LLGL::Key::LButton, [this]() { QueueClick(); });

	// All entities should have a transform
	EventFunc addedFunc = [this](Entity* entity)
	{
		OnEntityAdded(entity);
	};
	sceneManager.ConnectOnConstruct<TransformComponent>(addedFunc);
	EventFunc destroyFunc = [this](Entity* entity)
	{
		OnEntityRemoved(entity);
	};
	sceneManager.ConnectOnDestroy<TransformComponent>(destroyFunc);
}

void EntityMenu::Render(MonScene* scene, const RenderContext& renderContext)
{
	if (mSelectedEntity == nullptr)
	{
		RenderEntitySelection(scene);
	}
	else
	{
		RenderSelectedEntityMenu(scene);
	}

	if (mQueuedClick)
	{
		_OnClick(renderContext, scene->GetCamera());
		mQueuedClick = false;
	}
}

void EntityMenu::OnEntityAdded(Entity* entity)
{
	mEntityList.push_back(entity->GetId());
	items.push_back(entity->GetName().c_str());
}

void EntityMenu::OnEntityRemoved(Entity* entity)
{
	const auto it = std::find(mEntityList.begin(), mEntityList.end(), entity->GetId());
	if (it != mEntityList.end())
	{
		const int index = it - mEntityList.begin();
		mEntityList.erase(it);
		items.erase(items.begin() + index);
	}
}

void EntityMenu::RenderSelectedEntityMenu(MonScene* scene)
{
	if (mSelectedEntity == nullptr) return;

	ImGui::SetNextWindowPos(ImVec2(0, 20));
	ImGui::SetNextWindowSize(mSize);

	if (ImGui::Begin("Entity", &mOpen, mWindowFlags))
	{
		// Collapsible menu for each component
	}
	ImGui::End();
}

void EntityMenu::RenderEntitySelection(MonScene* scene)
{
	if (ImGui::Begin("Entity Selection", &mOpen, mWindowFlags))
	{
		if (scene == nullptr)
		{
			return;
		}

		ImGui::Combo("Entities", &current_entity_selected, items.data(), items.size());
		if (ImGui::Button("Open"))
		{
		}
	}
	ImGui::End();
}

void EntityMenu::QueueClick()
{
	mQueuedClick = true;
}

void EntityMenu::_HandleMouseMove(LLGL::Offset2D mousePos)
{
	mMousePos = {mousePos.x, mousePos.y};
}

glm::vec3 EntityMenu::_CalculateMouseRay(glm::vec2 mousePos, const RenderContext& renderContext,
                                         const Camera& camera) const
{
	const auto normalizeCoords = renderContext.NormalizedDeviceCoords({mousePos.x, mousePos.y, 1.0});
	const glm::vec4 homogenousClip = {normalizeCoords.x, normalizeCoords.y, -1.0f, 1.0f};
	glm::vec4 eyeRay = glm::inverse(renderContext.GetProjection()) * homogenousClip;
	eyeRay = glm::vec4(eyeRay.x, eyeRay.y, -1.0f, 0.0f);
	const auto ray = glm::normalize(glm::inverse(camera.GetView()) * eyeRay);
	return ray;
}

void EntityMenu::_OnClick(const RenderContext& renderContext, const Camera& camera)
{
	/*
	 *
	const auto ray = _CalculateMouseRay(mMousePos, renderContext, camera);
	//int i = 0;

	for (auto& map : mMapRegistry.GetAllMaps())
	{
		auto& data = map->GetData();
		const auto mapSize = map->GetMapSize();
		const auto position = map->GetPosition();

		const glm::vec3 mapTopLeft = {
			position.x - (mapSize.x / 2),
			position.y + (mapSize.y / 2),
			position.z
		};
		if (const auto t = _WithinMapBounds(mapTopLeft, mapSize, ray); t >= 0)
		{
			//mCurrentFocusedMapIndex = i;
			for (int i = 0; i < data.size(); i++)
			{
				glm::vec3 tilePos;
				glm::vec3 size;
				_CalculateTileInteractionData(*map, map->GetColumns(), i, tilePos, size);

				const auto cameraPos = camera.GetPosition();
				const auto intersection = glm::vec3(cameraPos.x + ray.x * t, cameraPos.y + ray.y * t,
				                                    cameraPos.z + ray.z * t);
				if (_Intersect(tilePos, size, intersection))
				{
					map->UpdateTile(i, mCurrentBrushIndex);
				}
			}
		}
		//i++;
	}
	 */
}
