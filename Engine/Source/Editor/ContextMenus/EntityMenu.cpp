#include "Input/InputHandler.h"
#include "Entity/Components/TransformComponent.h"
#include "Graphics/Core/ResourceManager.h"

#include "EntityMenu.h"

EntityMenu::EntityMenu(EntityRegistry& entityRegistry, InputHandler& inputHandler)
{
	// TODO: Handle mouse hover + selection
	inputHandler.RegisterMouseMoveHandler([this](LLGL::Offset2D mousePos) { _HandleMouseMove(mousePos); });
	inputHandler.RegisterButtonDownHandler(LLGL::Key::LButton, [this]() { _OnClick(); });

	// All entities should have a transform
	entityRegistry.GetEnttRegistry().on_construct<TransformComponent>().connect<&EntityMenu::OnEntityAdded>(this);
	entityRegistry.GetEnttRegistry().on_destroy<TransformComponent>().connect<&EntityMenu::OnEntityRemoved>(this);
}

void EntityMenu::RenderSelectedEntityMenu(EntityRegistry& entityRegistry, const ResourceManager& resourceManager)
{
	if (mSelectedEntity == 0) return;

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

void EntityMenu::OnEntityAdded(EnTTRegistry& registry, EntityId entity)
{
}

void EntityMenu::OnEntityRemoved(EnTTRegistry& registry, EntityId entity)
{
}

void EntityMenu::_HandleMouseMove(LLGL::Offset2D mousePos)
{
	mMousePos = {mousePos.x, mousePos.y};
}
