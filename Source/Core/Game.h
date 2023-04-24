#pragma once

class EntityRegistry;
class InputHandler;
class InputManager;
class GUISystem;
class LevelManager;
class MainGameGUI;
class MapSystem;
class Renderer;
class ResourceManager;
class Window;

int main(int argc, char** argv);

class Game
{
public:
	Game() = default;
	~Game() = default;

	Window& GetWindow() const;

	void ConfigureBaseGame();

	void CloseGame();

protected:
	void _DrawAxis();
	void RunGame();

private:
	std::unique_ptr<Renderer> mRenderer;
	std::unique_ptr<Window> mWindow;

	std::shared_ptr<InputHandler> mInputHandler;
	std::unique_ptr<InputManager> mInputManager;

	std::unique_ptr<MapSystem> mMapSystem;
	std::unique_ptr<ResourceManager> mResourceManager;

	std::unique_ptr<LevelManager> mLevelManager;
	std::unique_ptr<GUISystem> mGUISystem;

	std::unique_ptr<MainGameGUI> mMainGameGUI;

	std::unique_ptr<EntityRegistry> mEntityRegistry;

	bool mRunning = true;

	friend int ::main(int argc, char** argv);
};
