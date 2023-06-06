#pragma once

class CollisionSystem;
class EntityRegistry;
class InputHandler;
class InputManager;
class GUISystem;
class LevelManager;
class MainGameGUI;
class MapSystem;
class Renderer;
class ResourceManager;
class PhysicsSystem;
class PlayerSystem;
class Window;

struct Timer;

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
	void RunGame() const;
	void Update() const;

private:
	std::unique_ptr<Renderer> mRenderer;
	std::unique_ptr<Window> mWindow;
	std::unique_ptr<Timer> mTimer;

	std::shared_ptr<InputHandler> mInputHandler;
	std::unique_ptr<InputManager> mInputManager;

	std::unique_ptr<ResourceManager> mResourceManager;

	std::unique_ptr<LevelManager> mLevelManager;
	std::unique_ptr<GUISystem> mGUISystem;

	std::unique_ptr<MainGameGUI> mMainGameGUI;

	std::unique_ptr<EntityRegistry> mEntityRegistry;

	// systems
	std::unique_ptr<CollisionSystem> mCollisionSystem;
	std::unique_ptr<MapSystem> mMapSystem;
	std::unique_ptr<PhysicsSystem> mPhysicsSystem;
	std::unique_ptr<PlayerSystem> mPlayerSystem;

	bool mRunning = true;

	friend int ::main(int argc, char** argv);
};
