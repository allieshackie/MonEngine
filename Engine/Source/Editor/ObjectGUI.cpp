#include <imgui.h>
#include "Entity/EntityRegistry.h"
#include "Entity/Components/TransformComponent.h"

#include "ObjectGUI.h"

void ObjectGUI::RenderGUI(EntityRegistry& entityRegistry)
{
	ImGui::SetNextWindowPos(ImVec2(0, 20));
	ImGui::SetNextWindowSize(mSize);
	if (ImGui::Begin("Object Viewer", &mOpen, mWindowFlags))
	{
		const auto view = entityRegistry.GetEnttRegistry().view<TransformComponent>();
		view.each([=](auto& transform)
		{
			_ObjectTransform(transform);
		});
	}
	ImGui::End();
}

void ObjectGUI::_ObjectTransform(TransformComponent& transform)
{
	const auto rot = transform.mRotation;
	float tempPosition[3] = {rot.x, rot.y, rot.z};
	if (ImGui::InputFloat3("Rotation", tempPosition))
	{
		transform.mRotation = {tempPosition[0], tempPosition[1], tempPosition[2]};
	}
}
