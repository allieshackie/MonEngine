#include "App.h"

#include "Core/EngineContext.h"
#include "LLGL/Types.h"
#include "LLGL/Utils/ColorRGBA.h"

int main()
{
	const auto app = new App();
	const auto engine = std::make_unique<EngineContext>(app, LLGL::Extent2D{800, 600}, "",
	                                                    LLGL::ColorRGBAf{0.0f, 0.0f, 0.0f, 0.0f}, true, true);

	engine->Run(app);

	return 0;
}

void App::Init(EngineContext* engine)
{
	mEngine = engine;
}

void App::StartGame()
{
	// TODO: Re-add font, make sure to have font for gui
	//mEngine->LoadFont("PixelLettersFull.ttf");
	//mEngine->SetBackgroundClearColor({0.1f, 0.1f, 0.1f});

	// TODO: Data drive theming to make it easier to have different styles
	//GUISystem::LoadGUITheme();
	mEngine->LoadScene("app.json");

	mEngine->ToggleEditorMode(true);
}

void App::Update(float dt) const
{
}

void App::Render()
{
}

void App::RegisterEntityDescriptions() const
{
}

void App::SetSceneCallbacks(const SceneManager& sceneManager) const
{
}
