#include "Core/EngineContext.h"
#include "Entity/Descriptions/InteractiveDescription.h"
#include "Graphics/Debug/DebugDraw.h"
#include "GUI/GameGUI.h"

#include "Game.h"

int main()
{
	const auto engine = std::make_unique<EngineContext>();
	const auto game = new Game();

	engine->Run(game);

	return 0;
}

void Game::Init(EngineContext* engine)
{
	mEngine = engine;
	auto gameGUI = std::make_unique<GameGUI>(*mEngine);
	mEngine->SetGUIMenu(std::move(gameGUI));
}

void Game::Update(float dt) const
{
	//mPhysicsSystem->Update(mTimer->mDT);
}

void Game::RegisterEntityDescriptions() const
{
	mEngine->RegisterDescription<InteractiveDescription>(InteractiveDescription::JsonName);
}
