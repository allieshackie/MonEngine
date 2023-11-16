#include "Core/EngineContext.h"
#include "Entity/Descriptions/InteractiveDescription.h"
#include "GUI/GameGUI.h"

#include "Game.h"

int main()
{
	const auto game = new Game();
	const auto engine = std::make_unique<EngineContext>(game, LLGL::Extent2D{800, 600}, "Sandbox Game",
	                                                    LLGL::ColorRGBAf{1.0f, 0.5f, 1.0f}, false);

	engine->Run(game);

	return 0;
}

void Game::Init(EngineContext* engine)
{
	mEngine = engine;
	mEngine->UseGUIModule();
	mEngine->LoadFont("PixelLettersFull.ttf");
	mEngine->SetBackgroundClearColor({0.1f, 0.1f, 0.1f});

	auto gameGUI = std::make_unique<GameGUI>(*mEngine);
	mEngine->SetGUIMenu(std::move(gameGUI));
}

void Game::Update(float dt) const
{
	//mPhysicsSystem->Update(mTimer->mDT);
}

void Game::Render()
{
	mEngine->DrawText2D("Allie", {100, 100}, {2, 2}, {0.3f, 0.2f, 1.0f, 0.4f});
	mEngine->DrawCircle({100, 100, 0}, 10.0f, {1.0f, 1.0f, 1.0f, 0.2f});
	mEngine->DrawCircle({100, 150, 0}, 10.0f, {1.0f, 1.0f, 1.0f, 1.0f});

	mEngine->DrawBox({200, 150, 0}, {10.0f, 100.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f});
}

void Game::RegisterEntityDescriptions() const
{
	mEngine->RegisterDescription<InteractiveDescription>(InteractiveDescription::JsonName);
}
