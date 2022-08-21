#pragma once

class Camera;
class DescriptionRegistry;
class InputHandler;
class GUISystem;
class Renderer;
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
	std::unique_ptr<DescriptionRegistry> mDescriptionRegistry;
	std::unique_ptr<GUISystem> mGUISystem;
	std::unique_ptr<InputHandler> mInputHandler;
	Renderer* mRenderer = nullptr;
	std::unique_ptr<TileSetEditor> mTileSetEditor;
	std::unique_ptr<Window> mWindow;
	std::unique_ptr<Camera> mCamera;

	bool mRunning = true;

	friend int ::main(int argc, char** argv);
};

