#pragma once

class Camera;
class InputHandler;
class InputManager;
class InteractionManager;
class GUISystem;
class MapEditor;
class Renderer;
class ResourceManager;
class TileSetEditor;
class Window;

int main(int argc, char** argv);

class Game
{
public:
	Game() = default;
	~Game() = default;

	Window& getWindow() const;

	void configureLevel();

	//void registerEntityTemplates();

	void closeGame();

protected:
	void runGame() const;

private:
	std::unique_ptr<Renderer> mRenderer;
	std::unique_ptr<Window> mWindow;
	std::unique_ptr<Camera> mCamera;

	std::shared_ptr<InputHandler> mInputHandler;
	std::unique_ptr<InputManager> mInputManager;
	std::unique_ptr<InteractionManager> mInteractionManager;
	std::unique_ptr<ResourceManager> mResourceManager;

	std::unique_ptr<GUISystem> mGUISystem;
	std::unique_ptr<TileSetEditor> mTileSetEditor;
	std::unique_ptr<MapEditor> mMapEditor;

	bool mRunning = true;

	friend int ::main(int argc, char** argv);
};
