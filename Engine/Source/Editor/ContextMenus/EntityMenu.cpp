#include "Input/InputHandler.h"
#include "Core/Scene.h"
#include "Entity/Components/TransformComponent.h"
#include "Graphics/Core/ResourceManager.h"

#include "EntityMenu.h"

EntityMenu::EntityMenu(MonScene* scene, InputHandler& inputHandler)
{
	// TODO: Handle mouse hover + selection
	inputHandler.RegisterMouseMoveHandler([this](LLGL::Offset2D mousePos) { _HandleMouseMove(mousePos); });
	inputHandler.RegisterButtonDownHandler(LLGL::Key::LButton, [this]() { _OnClick(); });

	// All entities should have a transform

	EventFunc addedFunc = [this](Entity* entity)
	{
		OnEntityAdded(entity);
	};
	scene->ConnectOnConstruct<TransformComponent>(addedFunc);
	EventFunc destroyFunc = [this](Entity* entity)
	{
		OnEntityRemoved(entity);
	};
	scene->ConnectOnDestroy<TransformComponent>(destroyFunc);
}

void EntityMenu::RenderSelectedEntityMenu(MonScene* scene, const ResourceManager& resourceManager)
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

void EntityMenu::RenderEntitySelection()
{
}

void EntityMenu::OnEntityAdded(Entity* entity)
{
}

void EntityMenu::OnEntityRemoved(Entity* entity)
{
}

void EntityMenu::_HandleMouseMove(LLGL::Offset2D mousePos)
{
	mMousePos = {mousePos.x, mousePos.y};
}
