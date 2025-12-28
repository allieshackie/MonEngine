#pragma once

#include <memory>
#include <vector>

#include "ISystem.h"

class SystemManager
{
public:
	template <typename T, typename... Args>
	std::shared_ptr<T> RegisterSystem(Args&&... args)
	{
		static_assert(std::is_base_of_v<ISystem, T>, "T must derive from ISystem");

		std::shared_ptr<T> system = std::make_unique<T>(std::forward<Args>(args)...);
		mSystems.emplace_back(system);

		return system;
	}

	void FixedUpdate(float dt) const
	{
		for (auto& sys : mSystems) sys->FixedUpdate(dt);
	}

	void Update(float dt) const
	{
		for (auto& sys : mSystems) sys->Update(dt);
	}

	void Render(std::weak_ptr<World> world) const
	{
		for (auto& sys : mSystems) sys->Render(world);
	}

	void RenderGUI() const
	{
		for (auto& sys : mSystems) sys->RenderGUI();
	}

private:
	std::vector<std::shared_ptr<ISystem>> mSystems;
};
