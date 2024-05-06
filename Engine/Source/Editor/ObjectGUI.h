#pragma once

class EntityRegistry;

struct TransformComponent;

class ObjectGUI
{
public:
	void RenderGUI(EntityRegistry& entityRegistry);

private:
	void _ObjectTransform(TransformComponent& transform);

	bool mOpen = true;
	ImVec2 mSize = {250, 300};
	ImGuiWindowFlags mWindowFlags = ImGuiWindowFlags_None;
};
