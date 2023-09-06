#include "Core/EngineContext.h"
#include "Entity/Descriptions/InteractiveDescription.h"
#include "Graphics/Debug/DebugDraw.h"

#include "Game.h"

int main()
{
	const auto app = std::make_unique<Game>();
	app->Run();

	return 0;
}

Game::Game()
{
	Init("D:/dev/MonDev/Game/Data/");
}

void Game::Update(float dt) const
{
	//mPhysicsSystem->Update(mTimer->mDT);
}

void Game::RegisterEntityDescriptions() const
{
	RegisterDescription<InteractiveDescription>(InteractiveDescription::JsonName);
}
