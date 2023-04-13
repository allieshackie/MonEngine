#pragma once

class EntityRegistry;
class Camera;
class InputHandler;
class InputManager;
class GUISystem;
class MainGameGUI;
class MapEditor;
class MapSystem;
class MapInteractionSystem;
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
	std::unique_ptr<Camera> mCamera;

	std::shared_ptr<InputHandler> mInputHandler;
	std::unique_ptr<InputManager> mInputManager;

	std::unique_ptr<MapSystem> mMapSystem;
	std::unique_ptr<MapInteractionSystem> mMapInteractionSystem;
	std::unique_ptr<ResourceManager> mResourceManager;

	std::unique_ptr<GUISystem> mGUISystem;
	std::unique_ptr<MapEditor> mMapEditor;
	std::unique_ptr<MainGameGUI> mMainGameGUI;

	std::unique_ptr<EntityRegistry> mEntityRegistry;

	bool mRunning = true;

	friend int ::main(int argc, char** argv);
};
