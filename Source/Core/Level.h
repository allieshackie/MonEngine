#pragma once
#include <nlohmann/json.hpp>
#include <glm/vec3.hpp>

class Camera;
class InputManager;
class MapSystem;

class Level
{
public:
	Level(const std::string& levelName, MapSystem& mapSystem, InputManager& inputManager);
	~Level() = default;

	std::shared_ptr<Camera>& GetCamera();

private:
	void _ParseJson(const nlohmann::json& json);

	static constexpr char CAMERA_STRING[] = "camera";
	static constexpr char POSITION_STRING[] = "position";
	static constexpr char FRONT_STRING[] = "front";
	static constexpr char UP_STRING[] = "up";

	static constexpr char PLAYER_SPAWN_STRING[] = "player_spawn";

	static constexpr char MAPS_STRING[] = "maps";
	static constexpr char NAME_STRING[] = "name";
	static constexpr char ROTATION_STRING[] = "rotation";
	static constexpr char TILE_SIZE_STRING[] = "tile_size";

	glm::vec3 mPlayerSpawn = {0, 0, 0};

	std::shared_ptr<Camera> mCamera;
	MapSystem& mMapSystem;
	InputManager& mInputManager;
};
