#include "Input/InputHandler.h"
#include "Core/World.h"
#include "Entity/Components/TransformComponent.h"
#include "Entity/Entity.h"
#include "Graphics/RenderSystem.h"
#include "Graphics/Core/ResourceManager.h"

#include "EntityMenu.h"

EntityMenu::EntityMenu(std::weak_ptr<InputHandler> inputHandler, std::weak_ptr<World> world,
                       RenderSystem& renderSystem) : mWorld(world), mRenderSystem(renderSystem)
{
	// TODO: Handle mouse hover + selection
	if (const auto inputHandlerPtr = inputHandler.lock())
	{
		inputHandlerPtr->RegisterMouseMoveHandler([this](LLGL::Offset2D mousePos) { _HandleMouseMove(mousePos); });
		inputHandlerPtr->RegisterButtonDownHandler(LLGL::Key::LButton, [this]() { QueueClick(); });
	}

	if (const auto sharedWorld = mWorld.lock())
	{
		// All entities should have a transform
		EventFunc addedFunc = [this](Entity* entity)
		{
			OnEntityAdded(entity);
		};
		sharedWorld->ConnectOnConstruct<TransformComponent>(addedFunc);
		EventFunc destroyFunc = [this](Entity* entity)
		{
			OnEntityRemoved(entity);
		};
		sharedWorld->ConnectOnDestroy<TransformComponent>(destroyFunc);
	}
}

void EntityMenu::Render(RenderSystem& renderSystem)
{
	ImGui::SeparatorText("Select Entity");
	RenderEntitySelection();
	ImGui::SeparatorText("Entity Transforms");
	RenderSelectedEntityMenu(renderSystem);

	if (mQueuedClick)
	{
		if (const auto world = mWorld.lock())
		{
			_OnClick(mRenderSystem, world->GetCamera());
		}
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

void EntityMenu::RenderSelectedEntityMenu(RenderSystem& renderSystem)
{
	const auto sharedWorld = mWorld.lock();

	if (!(sharedWorld && sharedWorld->GetRegistry().valid(mSelectedEntity)))
	{
		return;
	}

	auto entity = sharedWorld->GetEntityForId(mSelectedEntity);
	auto transform = entity->TryGetComponent<TransformComponent>();
	if (transform)
	{
		ImGui::Text("Position: ");
		if (ImGui::DragFloat("PositionX", &transform->mPosition.x, 0.1f, -100.0f, 100.0f)) 
		{
			renderSystem.UpdateLights();
		}
		if (ImGui::DragFloat("PositionY", &transform->mPosition.y, 0.1f, -100.0f, 100.0f))
		{
			renderSystem.UpdateLights();
		}
		if (ImGui::DragFloat("PositionZ", &transform->mPosition.z, 0.1f, -100.0f, 100.0f))
		{
			renderSystem.UpdateLights();
		}
		ImGui::NewLine();

		ImGui::Text("Rotation: ");
		if (ImGui::DragFloat("RotationX", &transform->mRotation.x, 0.1f, -100.0f, 100.0f))
		{
			renderSystem.UpdateLights();
		}
		if (ImGui::DragFloat("RotationY", &transform->mRotation.y, 0.1f, -100.0f, 100.0f))
		{
			renderSystem.UpdateLights();
		}
		if (ImGui::DragFloat("RotationZ", &transform->mRotation.z, 0.1f, -100.0f, 100.0f))
		{
			renderSystem.UpdateLights();
		}

		ImGui::NewLine();
		ImGui::Text("Size: ");
		if (ImGui::DragFloat("SizeX", &transform->mSize.x, 0.1f, 0.1f, 200.0f))
		{
			renderSystem.UpdateLights();
		}
		if (ImGui::DragFloat("SizeY", &transform->mSize.y, 0.1f, 0.1f, 200.0f))
		{
			renderSystem.UpdateLights();
		}
		if (ImGui::DragFloat("SizeZ", &transform->mSize.z, 0.1f, 0.1f, 200.0f))
		{
			renderSystem.UpdateLights();
		}
	}
}

void EntityMenu::RenderEntitySelection()
{
	if (mEntityList.empty())
	{
		return;
	}
	const auto sharedWorld = mWorld.lock();
	if (!(sharedWorld && sharedWorld->GetRegistry().valid(mSelectedEntity)))
	{
		mSelectedEntity = mEntityList[current_entity_selected];
		return;
	}

	if (ImGui::Combo("Entities", &current_entity_selected, items.data(), items.size()))
	{
		mSelectedEntity = mEntityList[current_entity_selected];
	}
}

void EntityMenu::QueueClick()
{
	mQueuedClick = true;
}

void EntityMenu::_HandleMouseMove(LLGL::Offset2D mousePos)
{
	mMousePos = {mousePos.x, mousePos.y};
}

glm::vec3 EntityMenu::_CalculateMouseRay(glm::vec2 mousePos, const RenderSystem& renderSystem,
                                         const Camera& camera) const
{
	/*
	const auto normalizeCoords = renderSystem.NormalizedDeviceCoords({mousePos.x, mousePos.y, 1.0});
	const glm::vec4 homogenousClip = {normalizeCoords.x, normalizeCoords.y, -1.0f, 1.0f};
	glm::vec4 eyeRay = glm::inverse(renderSystem.GetPerspectiveProjection()) * homogenousClip;
	eyeRay = glm::vec4(eyeRay.x, eyeRay.y, -1.0f, 0.0f);
	const auto ray = glm::normalize(glm::inverse(camera.GetView()) * eyeRay);
	*/
	return glm::vec3{};
}

void EntityMenu::_OnClick(const RenderSystem& renderSystem, const Camera& camera)
{
	/*
	 *
	const auto ray = _CalculateMouseRay(mMousePos, RenderSystem, camera);
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
