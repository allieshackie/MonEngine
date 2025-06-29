#include <imgui.h>
#include "Core/Scene.h"
#include "Entity/Components/ModelComponent.h"
#include "Entity/Components/TransformComponent.h"
#include "Graphics/Core/ResourceManager.h"

#include "ObjectGUI.h"

void ObjectGUI::RenderGUI(MonScene* scene, ResourceManager& resourceManager)
{
	ImGui::SetNextWindowPos(ImVec2(0, 20));
	ImGui::SetNextWindowSize(mSize);
	/*
	 *
	if (ImGui::Begin("Object Viewer", &mOpen, mWindowFlags))
	{
		const auto view = entityRegistry.GetEnttRegistry().view<TransformComponent>();
		view.each([=](auto& transform)
		{
			_ObjectTransform(transform);
		});
	}
	ImGui::End();
	 */

	if (ImGui::Begin("Bone Selection", &mOpen, mWindowFlags))
	{
		auto view = scene->GetRegistry().view<ModelComponent>();
		view.each([=, &resourceManager](auto& mesh)
		{
			/* TODO: figure out how to re-introduce
			 *
			if (!mesh.mHasBones)
			{
				return;
			}
			 */
			const auto model = resourceManager.GetModelFromId(mesh.mModelPath);
			auto& boneNamesToIndex = model.GetBoneNamesToIndex();

			// Extract keys into a vector
			keys.clear();
			for (const auto& kv : boneNamesToIndex)
			{
				keys.push_back(kv.first);
			}

			// Create a C-style array of const char* from the vector of keys
			items.clear();
			for (const std::string& key : keys)
			{
				items.push_back(key.c_str());
			}
			if (ImGui::Combo("Bones", &currentSelection, items.data(), items.size()))
			{
				mesh.mCurrentBoneIndex = boneNamesToIndex.find(keys[currentSelection])->second;
			}
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
