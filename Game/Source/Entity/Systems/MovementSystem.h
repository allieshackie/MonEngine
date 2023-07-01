#pragma once

class PlayerInputHandler;

class MovementSystem
{
public:
	MovementSystem(PlayerInputHandler& playerInputHandler);
	~MovementSystem() = default;

	void Update();
private:
	void _MovePlayer();
	void _MoveEntity();

	PlayerInputHandler& mPlayerInputHandler;
};
