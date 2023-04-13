#pragma once

class Camera;

class Level
{
public:
	Level();
	~Level() = default;

private:
	static constexpr char CAMERA_STRING[] = "camera";
	static constexpr char POSITION_STRING[] = "position";
	static constexpr char FORWARD_STRING[] = "forward";
	static constexpr char UP_STRING[] = "up";

	static constexpr char PLAYER_SPAWN_STRING[] = "player_spawn";

	static constexpr char MAPS_STRING[] = "maps";
	static constexpr char NAME_STRING[] = "name";
	static constexpr char SIZE_STRING[] = "size";
	static constexpr char ROTATION_STRING[] = "rotation";

	std::unique_ptr<Camera> mCamera;
};
