#pragma once

enum MovementMode
{
	None,
	Idle,
	Seek
};

struct BehaviorComponent
{
	MovementMode mMovement = MovementMode::None;
};
