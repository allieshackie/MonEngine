#pragma once
#include "Scene.h"

class SceneManager {
public:
	SceneManager();
	~SceneManager();

	void LoadScene();

	void LoadScenesFromJson();

private:
	std::unordered_map<char*, Scene> mScenes;
};