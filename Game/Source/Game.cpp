#include "Core/EngineContext.h"
#include "Entity/Descriptions/InteractiveDescription.h"
#include "Graphics/Debug/DebugDraw.h"
#include "GUI/GameGUI.h"

#include "Game.h"

int main()
{
	const auto app = std::make_unique<Game>();
	app->Run();

	return 0;
}

Game::Game()
{
	Init();
	_Init();
}

void Game::_Init()
{
	auto gameGUI = std::make_unique<GameGUI>(*this);
	SetGUIMenu(std::move(gameGUI));
}

void Game::Update(float dt) const
{
	//mPhysicsSystem->Update(mTimer->mDT);
}

void Game::RegisterEntityDescriptions() const
{
	RegisterDescription<InteractiveDescription>(InteractiveDescription::JsonName);
}
