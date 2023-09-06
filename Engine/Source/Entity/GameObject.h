#pragma once
#include "EntityRegistry.h"

class GameObject
{
public:
	void Render();

private:
	EntityId entityId;
};
