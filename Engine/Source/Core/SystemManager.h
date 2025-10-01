#pragma once

#include <memory>
#include <vector>

#include "ISystem.h"

class SystemManager
{
public:
	void RegisterSystem(std::unique_ptr<ISystem> system)
	{
		mSystems.emplace_back(std::move(system));
	}

	void FixedUpdate(double dt) const
	{
		for (auto& sys : mSystems) sys->FixedUpdate(dt);
	}

	void Update(double dt) const
	{
		for (auto& sys : mSystems) sys->Update(dt);
	}

	void Render() const
	{
		for (auto& sys : mSystems) sys->Render();
	}

private:
	std::vector<std::unique_ptr<ISystem>> mSystems;
};
