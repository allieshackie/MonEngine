#pragma once

class Camera;
class InputHandler;
class InputManager;
class GUISystem;
class MapEditor;
class Renderer;
class TileSetEditor;
class UIInputManager;
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
	Renderer* mRenderer = nullptr;
	std::unique_ptr<Window> mWindow;
	std::unique_ptr<Camera> mCamera;

	std::unique_ptr<InputHandler> mInputHandler;
	std::unique_ptr<InputManager> mInputManager;
	std::unique_ptr<UIInputManager> mUIInputManager;

	std::unique_ptr<GUISystem> mGUISystem;
	std::unique_ptr<TileSetEditor> mTileSetEditor;
	std::unique_ptr<MapEditor> mMapEditor;

	bool mRunning = true;

	friend int ::main(int argc, char** argv);
};
