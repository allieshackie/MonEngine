#pragma once

#include "Window.h"
#include "DescriptionRegistry.h"
#include "Map.h"

int main(int argc, char** argv);

class Game
{
public: 
	Game();
	virtual ~Game();

	//void onEvent(Event& e);

	Window& getWindow();

	void configureLevel();

	void registerDescriptions();
	void registerEntityTemplates();

	void closeGame();

private:
	void runGame();

private:
	std::unique_ptr<Window> mWindow;
	SDL_Renderer* mRenderer;
	std::unique_ptr<DescriptionRegistry> mDescriptionRegistry;
	//SceneManager mSceneManager;

	std::unique_ptr<Map> mMap;

	bool mRunning = true;

	friend int ::main(int argc, char** argv);
};

