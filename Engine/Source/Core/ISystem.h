#pragma once

class ResourceManager;
class World;

class ISystem
{
	// Not pure virtual since not every system will require all entry points
public:
	virtual ~ISystem() = default;

	virtual void FixedUpdate(float dt)
	{
	}

	virtual void Update(float dt)
	{
	}

	virtual void Render(std::weak_ptr<World> world)
	{
	}

	virtual void RenderGUI()
	{
	}
};
