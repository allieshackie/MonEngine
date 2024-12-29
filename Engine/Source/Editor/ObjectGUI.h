#pragma once

class MonScene;
class ResourceManager;

struct TransformComponent;

class ObjectGUI
{
public:
	void RenderGUI(MonScene* scene, ResourceManager& resourceManager);

private:
	void _ObjectTransform(TransformComponent& transform);

	bool mOpen = true;
	ImVec2 mSize = {250, 300};
	ImGuiWindowFlags mWindowFlags = ImGuiWindowFlags_None;

	int currentSelection = 0;
	std::vector<std::string> keys;
	std::vector<const char*> items;
};
