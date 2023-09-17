#pragma once
#include "Core/EngineContext.h"

class Game : public EngineContext
{
public:
	Game();
	~Game() override = default;

	void Update(float dt) const override;
	void RegisterEntityDescriptions() const override;
private:
	void _Init();
};
