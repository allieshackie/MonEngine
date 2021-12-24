#pragma once

class DescriptionRegistry;
class InputHandler;
class GUISystem;
class Map;
class Renderer;
class Texture;
class Window;

int main(int argc, char** argv);

class Game
{
public: 
	Game();
	virtual ~Game() = default;

	Window& getWindow() const;

	void configureLevel();

	//void registerEntityTemplates();

	void closeGame();

protected:
	void runGame() const;

private:
	std::unique_ptr<Window> mWindow;
	std::unique_ptr<Renderer> mRenderer;
	std::unique_ptr<InputHandler> mInputHandler;
	std::unique_ptr<DescriptionRegistry> mDescriptionRegistry;
	std::unique_ptr<GUISystem> mGUISystem;

	Texture* texture = nullptr;
	//SceneManager mSceneManager;

	std::unique_ptr<Map> mMap;

	bool mRunning = true;

	friend int ::main(int argc, char** argv);
};

