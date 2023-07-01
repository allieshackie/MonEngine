#pragma once

enum MovementInput
{
	None = 1 << 0,
	Up = 1 << 1,
	Down = 1 << 2,
	Left = 1 << 3,
	Right = 1 << 4,
	Forward = 1 << 5,
	Backward = 1 << 6,
};

struct PlayerComponent
{
	int mMovementInput = MovementInput::None;
};
